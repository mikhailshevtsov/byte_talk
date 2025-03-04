#include "connector.hpp"

namespace bt
{

bool connector::read_some()
{
    return m_read_buffer.read_some(get());
}

bool connector::write_some()
{
    return m_write_buffer.write_some(get());
}

void connector::load(std::vector<char>&& buffer)
{
    m_write_buffer.load(std::move(buffer));
}

void connector::load(std::string_view buffer)
{
    m_write_buffer.load(buffer);
}

std::string_view connector::read_buffer() const noexcept
{
    return m_read_buffer.get();
}

std::string_view connector::write_buffer() const noexcept
{
    return m_write_buffer.get();
}

}