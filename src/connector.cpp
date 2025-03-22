#include "connector.hpp"

namespace bt
{

bool connector::read_some(bool& is_completed)
{
    return io_some(m_read_buffer, ::read, is_completed);
}

bool connector::write_some(bool& is_completed)
{
    return io_some(m_write_buffer, ::write, is_completed);
}

bool connector::push(std::string_view buffer)
{
    if (m_write_buffer.size > 0)
        return false;

    m_write_buffer.storage.resize(buffer.size());
    std::copy(std::cbegin(buffer), std::cend(buffer), std::begin(m_write_buffer.storage));
    m_write_buffer.size = htonl(static_cast<uint32_t>(buffer.size()));
    m_write_buffer.bytes = 0;

    return true;
}

std::string_view connector::read_buffer() const noexcept
{
    return std::string_view(m_read_buffer.storage.data(), m_read_buffer.size);
}

std::string_view connector::write_buffer() const noexcept
{
    return std::string_view(m_write_buffer.storage.data(), m_write_buffer.size);
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