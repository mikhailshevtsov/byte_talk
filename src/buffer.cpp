#include "buffer.hpp"

#include <algorithm>

namespace bt
{

bool buffer::read_some(int sockfd)
{
    return io_some(sockfd, ::read);
}

bool buffer::write_some(int sockfd)
{
    return io_some(sockfd, ::write);
}

void buffer::load(std::vector<char>&& _buffer)
{
    m_buffer = std::move(_buffer);
    m_buffer_size = htonl(static_cast<uint32_t>(m_buffer.size()));
    m_bytes = 0; 
}

void buffer::load(std::string_view _buffer)
{
    m_buffer.resize(_buffer.size());
    std::copy(std::cbegin(_buffer), std::cend(_buffer), std::begin(m_buffer));
}

std::string_view buffer::get() const noexcept
{
    return std::string_view(m_buffer.data(), m_buffer.size());
}

}