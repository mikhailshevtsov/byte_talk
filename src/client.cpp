#include <byte_talk/client.hpp>
#include <byte_talk/server.hpp>

#include <utility>

namespace bt
{

client::client(bt::server* server, std::size_t index)
    : m_server{server}
    , m_index{index}
{
    if (valid())
        m_server->get_connection(m_index).inc();
}

client::client() = default;

client::client(const client& other)
    : m_server{other.m_server}
    , m_index{other.m_index}
{
    if (valid())
        m_server->get_connection(m_index).inc();
}

client& client::operator=(const client& other)
{
    client temp(other);
    swap(temp);
    return *this;
}

client::client(client&& other) noexcept
    : m_server(std::exchange(other.m_server, nullptr))
    , m_index(std::exchange(other.m_index, 0))
{}

client& client::operator=(client&& other) noexcept
{
    client temp(std::move(other));
    swap(temp);
    return *this;
}

client::~client()
{
    if (valid() && m_server->get_connection(m_index).dec() == 1)
        m_server->free_connection(m_index);
}

bool client::operator==(const client& other) const
{
    return m_server == other.m_server && m_index == other.m_index;
}

bool client::operator!=(const client& other) const
{
    return !(*this == other);
}

bool client::valid() const
{
    return m_server;
}

bool client::alive() const
{
    return valid() && m_server->get_connection(m_index).sock.valid();
}

bt::server* client::server() { return m_server; }

const bt::server* client::server() const { return m_server; }

std::size_t client::index() const
{
    return m_index;
}

std::size_t client::count() const
{
    if (!valid())
        return 0;
    return m_server->get_connection(m_index).count;
}

int client::socket() const
{
    if (!valid())
        return -1;
    return m_server->get_connection(m_index).sock.fd();
}

std::string_view client::read() const
{
    if (!valid())
        return {};
    return m_server->get_connection(m_index).rbuf;
}

void client::send(std::string_view msg)
{
    if (!valid())
        return;
    m_server->get_connection(m_index).wbuf.push(msg);
    send();
}

void client::send()
{
    m_server->set_writing(m_index, true);
}

std::size_t client::bytes_to_send() const
{
    if (!valid())
        return 0;
    return m_server->get_connection(m_index).wbuf.size();
}

bool client::sent_all() const
{
    return bytes_to_send() == 0;
}

const std::any& client::data() const
{
    return m_server->get_connection(m_index).data;
}

std::any& client::data()
{
    return m_server->get_connection(m_index).data;
}

void client::set_data(const std::any& data)
{
    if (!valid())
        return;
    m_server->get_connection(m_index).data = data;
}

void client::set_data(std::any&& data)
{
    if (!valid())
        return;
    m_server->get_connection(m_index).data = std::move(data);
}

void client::close()
{
    if (!valid())
        return;
    m_server->close(m_index);
}

void client::swap(client& other)
{
    std::swap(m_server, other.m_server);
    std::swap(m_index, other.m_index);
}

}