#include <byte_talk/server.hpp>

namespace bt
{

server::server(short port)
    : m_port{port}
    , m_events(MAX_EVENTS)
    , m_connections(MAX_EVENTS)
{}

void server::stop()
{
    m_is_running = false;
}

bool server::is_running() const
{
    return m_is_running;
}

int server::run()
{
    if (!setup())
    {
        print_error();
        return -1;
    }

    m_is_running = true;
    while (m_is_running)
    {
        if (!loop())
            print_error();
    }

    return 0;
}

bool server::setup()
{
    int res = 0;

    m_acceptor = net::make_socket();
    if (!m_acceptor.valid())
        return false;

    res = m_acceptor.set_nonblocking(true);
    if (res < 0)
        return false;

    res = m_acceptor.bind(m_port);
    if (res < 0)
        return false;

    res = m_acceptor.listen();
    if (res < 0)
        return false;

    m_epoll = net::epoll::create();
    if (!m_epoll.valid())
        return false;
    
    res = m_epoll.add(m_acceptor, {EPOLLIN, std::size_t(-1)});
    if (res < 0)
        return false;

    return true;
}


bool server::loop()
{
    int res = m_epoll.wait(m_events.data(), m_events.size());
    for (int i = 0; i < res; ++i)
    {
        // event on listening socket
        if (m_events[i].index() == std::size_t(-1))
        {
            net::socket sock = m_acceptor.accept();
            if (!sock.valid())
                continue;

            res = sock.set_nonblocking(true);
            if (res < 0)
                continue;

            std::size_t index = m_connections.acquire();
            
            res = m_epoll.add(sock, {EVENTS, index});
            if (res < 0)
            {
                disconnect(index);
                continue;
            }

            auto& conn = get_connection(index);
            conn.start(std::move(sock));
            conn.inc();

            on_connected(client(this, index));

            continue;
        }

        std::size_t index = m_events[i].index();
        auto events = m_events[i].events();
        auto& conn = get_connection(index);

        // close event
        if (events & EPOLLHUP || events & EPOLLRDHUP || events & EPOLLERR)
        {
            disconnect(index);
            continue;
        }

        // input event
        if (events & EPOLLIN)
        {
            if (conn.read())
            {
                on_read(client(this, index));
                conn.rbuf.clear();
            }
            else
                disconnect(index);
        }

        // output event
        if (events & EPOLLOUT)
        {
            if (conn.write())
            {
                on_sent(client(this, index));
                if (conn.wbuf.empty())
                    set_writing(index, false);
            }
            else
                disconnect(index);
        }
    }

    return res >= 0;
}

void server::set_writing(std::size_t index, bool value)
{
    uint32_t events = EVENTS;
    if (value) events |= EPOLLOUT;
    connection& conn = get_connection(index);

    int res = m_epoll.mod(conn.sock, {events, index});
    if (res < 0)
        disconnect(index);
}

connection& server::get_connection(std::size_t index)
{
    return m_connections[index];
}

void server::free_connection(std::size_t index)
{
    m_connections.release(index);
}

void server::disconnect(std::size_t index)
{
    auto& conn = get_connection(index);
    if (!conn.sock.valid())
        return;
    on_disconnected(client(this, index));
    m_epoll.del(conn.sock);
    conn.stop();
    if (conn.dec() == 1)
        free_connection(index);
}

void server::print_error() const
{
    perror("Error: ");
}

}