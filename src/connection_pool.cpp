#include <byte_talk/connection_pool.hpp>

namespace bt
{

connection_pool::connection_pool(std::size_t size)
    : _connections(size)
{
    for (std::size_t i = size; i > 0;)
        _free_indices.push(--i);
}

connection* connection_pool::acquire()
{
    if (full())
        return nullptr;
    std::size_t index = _free_indices.top();
    _free_indices.pop();
    return &_connections[index];
}

void connection_pool::release(std::size_t index)
{
    if (index < size())
        _free_indices.push(index);
}

void connection_pool::release(connection* conn)
{
    release(index_of(conn));
}

connection& connection_pool::operator[](std::size_t index)
{
    return _connections[index];
}

std::size_t connection_pool::size() const
{
    return _connections.size();
}

std::size_t connection_pool::free_size() const
{
    return _free_indices.size();
}

std::size_t connection_pool::full() const
{
    return _free_indices.empty();
}

std::size_t connection_pool::index_of(connection* conn) const
{
    return conn - _connections.data();
}

}

