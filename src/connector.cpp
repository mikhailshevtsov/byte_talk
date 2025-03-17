#include "connector.hpp"

namespace bt
{

bool connector::read_some(bool& is_completed)
{
    return io_some(m_read_buffer_size, m_read_bytes, m_read_buffer, ::read, is_completed);
}

bool connector::write_some(bool& is_completed)
{
    return io_some(m_write_buffer_size, m_write_bytes, m_write_buffers_queue.front(), ::write, is_completed);
}

void connector::push(std::vector<char>&& buffer)
{
    m_write_buffer_size = htonl(static_cast<uint32_t>(buffer.size()));
    m_write_buffers_queue.push(std::move(buffer));
}

void connector::push(std::string_view buffer)
{
    push(std::vector<char>(std::cbegin(buffer), std::cend(buffer)));
}

void connector::pop()
{
    m_write_buffers_queue.pop();
}

size_t connector::queue_size() const noexcept
{
    return m_write_buffers_queue.size();
}

std::string_view connector::read_buffer() const noexcept
{
    return std::string_view(m_read_buffer.data(), m_read_buffer.size());
}

std::string_view connector::write_buffer() const noexcept
{
    return std::string_view(m_write_buffers_queue.front().data(), m_write_buffers_queue.front().size());
}

bool connector::want_read() const noexcept
{
    return m_flags & read_flag;
}

bool connector::want_write() const noexcept
{
    return m_flags & read_flag;
}

void connector::set_want_read(bool value) noexcept
{
    if (value)
        m_flags |= read_flag;
    else
        m_flags &= ~read_flag;
}

void connector::set_want_write(bool value) noexcept
{
    if (value)
        m_flags |= write_flag;
    else
        m_flags &= ~read_flag;
}

}