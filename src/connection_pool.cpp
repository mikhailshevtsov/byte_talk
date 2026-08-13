#include <byte_talk/connection_pool.hpp>

namespace bt
{

connection_pool::connection_pool(std::size_t size)
    : m_connections(size)
{
    for (std::size_t i = 0; i < size; ++i)
        m_free_indices.push(i);
}

std::size_t connection_pool::acquire()
{
    std::lock_guard<std::mutex> lg(m_mtx);
    if (full())
        return std::size_t(-1);
    std::size_t free_index = m_free_indices.front();
    m_free_indices.pop();
    return free_index;
}

void connection_pool::release(std::size_t index)
{
    std::lock_guard<std::mutex> lg(m_mtx);
    m_free_indices.push(index);
}

connection& connection_pool::operator[](std::size_t index)
{
    return m_connections[index];
}

std::size_t connection_pool::size() const
{
    return m_connections.size();
}

std::size_t connection_pool::free_size() const
{
    return m_free_indices.size();
}

std::size_t connection_pool::full() const
{
    return m_free_indices.empty();
}

}

