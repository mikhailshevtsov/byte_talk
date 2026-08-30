#include <byte_talk/client.hpp>
#include <byte_talk/server.hpp>

#include <utility>

namespace bt
{

client::client(bt::server* _server, connection* conn)
    : _server{_server}
    , _conn{conn}
    , _age{conn->age}
{}

client::client() = default;

client::operator bool() const
{
    return valid();
}

bool client::valid() const
{
    return _server && _conn;
}

bool client::alive() const
{
    return valid() && _conn->alive && _conn->age == _age;
}

bt::server* client::server() const
{
    return _server;
}

int client::socket() const
{
    if (!alive())
        return -1;
    return _conn->sock.get();
}

std::string_view client::read() const
{
    if (!alive())
        return {};
    return _conn->rbuf;
}

std::size_t client::bytes_to_send() const
{
    if (!alive())
        return 0;
    return _conn->wbuf.size();
}

bool client::sent_all() const
{
    if (!alive())
        return false;
    return bytes_to_send() == 0;
}

const void* client::data() const
{
    if (!alive())
        return nullptr;
    return _conn->data;
}

void* client::data()
{
    if (!alive())
        return nullptr;
    return _conn->data;
}

void client::set_data(void* data)
{
    if (!alive())
        return;
    _conn->data = data;
}

void client::set_timeout(timeout_t timeout)
{
    if (!alive())
        return;
    _server->set_timeout(*this, timeout);
}

deadline_t client::deadline() const
{
    if (!alive())
        return {};
    return _conn->deadline;
}

std::size_t client::index() const
{
    if (!alive())
        return -1;
    return _server->index_of(*this);
}

void client::send(std::string_view msg)
{
    if (!alive())
        return;
    _server->send_to(*this, msg);
}

void client::send()
{
    if (!alive())
        return;
    _server->send_to(*this);
}

void client::close()
{
    if (!alive())
        return;
    _server->close(*this);
}

}