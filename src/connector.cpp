#include "connector.hpp"

namespace bt
{

bool connector::read_some()
{
    return io_some(m_read_buffer_size, m_read_bytes, m_read_buffer, ::read);
}

bool connector::write_some()
{
    return io_some(m_write_buffer_size, m_write_bytes, m_write_buffers_queue.front(), ::write);
}

void connector::push(std::vector<char>&& buffer)
{
    m_write_buffers_queue.push(std::move(buffer));
    if (m_write_buffers_queue.size() > MAX_QUEUE_SIZE)
        pop();
}

void connector::push(std::string_view buffer)
{
    push(std::vector<char>(std::cbegin(buffer), std::cend(buffer)));
}

void connector::pop()
{
    m_write_buffers_queue.pop();
}

std::string_view connector::read_buffer() const noexcept
{
    return std::string_view(m_read_buffer.data(), m_read_buffer.size());
}

std::string_view connector::write_buffer() const noexcept
{
    return std::string_view(m_write_buffers_queue.front().data(), m_write_buffers_queue.front().size());
}

}