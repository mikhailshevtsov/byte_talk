#include <byte_talk/server.hpp>

#include <iostream>

namespace bt
{

server::server(const bt::config& cfg)
    : _cfg{cfg}
    , _events(1024)
    , _connections(cfg.max_conn)
{}

const bt::config& server::config() const
{
    return _cfg;
}

int server::run()
{
    int res = 0;

    res = setup();
    if (res < 0)
    {
        print_error();
        return res;
    }

    while (true)
        loop();

    return 0;
}

int server::setup()
{
    int res = 0;

    _acceptor = net::make_socket();
    if (!_acceptor)
        return -1;

    res = _acceptor.set_nonblocking(true);
    if (res < 0)
        return res;

    res = _acceptor.bind(_cfg.port);
    if (res < 0)
        return res;

    res = _acceptor.listen();
    if (res < 0)
        return res;

    _epoll = net::epoll::create();
    if (!_epoll)
        return -1;
    
    res = _epoll.add(_acceptor, {EPOLLIN, &_acceptor});
    if (res < 0)
        return res;

    if (_cfg.ssl)
    {
        _ssl_ctx = net::make_ssl_context(_cfg.cert.c_str(), _cfg.pkey.c_str());
        if (!_ssl_ctx)
            return -1;
    }

    _timer = net::make_timer(_interval);
    if (!_timer)
        return -1;

    res = _epoll.add(_timer, {EPOLLIN, &_timer});
    if (res < 0)
        return res;

    return 0;
}


int server::loop()
{
    int res = _epoll.wait(_events.data(), _events.size());
    for (int i = 0; i < res; ++i)
    {
        auto event = _events[i];
        auto conn = static_cast<connection*>(event.data());

        if (is_timer_fired(event))
        {
            handle_event(&server::timer_fired, conn);
            continue;
        }

        if (is_opened(event))
        {
            handle_event(&server::opened, conn);
            continue;
        }

        if (is_closed(event))
        {
            handle_event(&server::closed, conn);
            continue;
        }

        if (is_read(event))
        {
            handle_event(&server::read, conn);
        }

        if (is_sent(event))
        {
            handle_event(&server::sent, conn);
        }
    }

    return res;
}

int server::timer_fired(connection*)
{
    int res = 0;

    uint64_t expirations{};
    res = _timer.read(expirations);
    if (res < 0)
        return res;

    while (!_timeouts.empty())
    {
        auto [conn, age, deadline] = _timeouts.top();
        if (now() >= deadline)
        {
            if (conn->age == age && conn->deadline == deadline)
                closed(conn);
            _timeouts.pop();
        }
        else
            break;
    }

    return res;
}

int server::opened(connection*)
{
    int res = 0;

    auto conn = _connections.acquire();
    if (!conn)
    {
        std::cerr << "Maximum connections limit exceeded\n";
        return res;
    }

    net::socket sock = _acceptor.accept();
    if (!sock)
        return -1;

    res = sock.set_nonblocking(true);
    if (res < 0)
        return res;

    net::ssl ssl;
    if (_cfg.ssl)
    {
        ssl = net::make_ssl(_ssl_ctx);
        if (!ssl)
            return -1;

        res = ssl.set_socket(sock);
        if (res < 0)
            return res;
    }

    conn->revive(std::move(sock), std::move(ssl));

    using namespace std::chrono_literals;
    set_timeout(conn, 0s);

    res = _epoll.add(conn->sock, {EVENTS, conn});
    if (res < 0)
        return res;

    on_opened(client(this, conn));

    return res;
}

int server::closed(connection* conn)
{
    int res = 0;

    if (!conn && !conn->alive)
        return res;

    on_closed(client(this, conn));

    conn->kill();

    _connections.release(conn);

    return res;
}

int server::read(connection* conn)
{
    ssize_t n = 0;

    if (!conn || !conn->alive)
        return n;

    n = conn->read();
    if (n >= 0)
    {
        on_read(client(this, conn));
        conn->rbuf.clear();
    }
    else
        closed(conn);
    
    return n;
}

int server::sent(connection* conn)
{
    ssize_t n = 0;

    if (!conn || !conn->alive)
        return n;

    n = conn->write();
    if (n >= 0)
    {
        on_sent(client(this, conn));
        if (conn->wbuf.empty())
            set_writing(conn, false);
    }
    else
        closed(conn);

    return n;
}

int server::handle_event(int(server::*callback)(connection*), connection* conn)
{
    int res = (this->*callback)(conn);
    if (res < 0)
        print_error();
    return res;
}

void server::set_timeout(connection* conn, timeout_t timeout)
{
    if (!conn || !conn->alive)
        return;
    conn->deadline = now() + timeout;
    _timeouts.emplace(conn, conn->age, conn->deadline);
}

void server::set_writing(connection* conn, bool value)
{
    if (!conn || !conn->alive)
        return;

    uint32_t events = EVENTS;
    if (value) events |= EPOLLOUT;
    
    int res = _epoll.mod(conn->sock, {events, conn});
    if (res < 0)
        closed(conn);
}

void server::print_error() const
{
    perror("Error");
}

bool server::is_timer_fired(const net::epoll::event& event) const { return event.data() == &_timer; }
bool server::is_opened(const net::epoll::event& event) const { return event.data() == &_acceptor; }
bool server::is_closed(const net::epoll::event& event) const { return event.events() & EPOLLHUP || event.events() & EPOLLRDHUP || event.events() & EPOLLERR; }
bool server::is_read(const net::epoll::event& event) const { return event.events() & EPOLLIN; }
bool server::is_sent(const net::epoll::event& event) const { return event.events() & EPOLLOUT; }

void server::send_to(client _client, std::string_view msg)
{
    if (!_client.alive())
        return;
    auto conn = _client._conn;
    conn->wbuf.push(msg);
    set_writing(conn, true);
}

void server::send_to(client _client)
{
    if (!_client.alive())
       return;
    set_writing(_client._conn, true);
}

void server::close(client _client)
{
    closed(_client._conn);
}

void server::set_timeout(client _client, timeout_t timeout)
{
    set_timeout(_client._conn, timeout);
}

std::size_t server::index_of(client _client) const
{
    return _connections.index_of(_client._conn);
}

}