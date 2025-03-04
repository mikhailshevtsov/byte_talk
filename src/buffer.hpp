#ifndef BYTETALK_BUFFER_HPP
#define BYTETALK_BUFFER_HPP

#include <vector>
#include <string>
#include <cstdint>

#include <unistd.h>
#include <arpa/inet.h>

namespace bt
{

class buffer
{
public:
    bool read_some(int sockfd);
    bool write_some(int sockfd);

    void load(std::vector<char>&& _buffer);
    void load(std::string_view _buffer);

    std::string_view get() const noexcept;

    static constexpr uint32_t SIZE_BYTES = 4;

private:
    template <typename Func>
    int io_some(int sockfd, Func&& func);

private:
    std::vector<char> m_buffer;
    uint32_t m_buffer_size = 0;
    uint32_t m_bytes = 0;
};

template <typename Func>
int buffer::io_some(int sockfd, Func&& func)
{
    if (m_bytes < SIZE_BYTES)
    {
        int n = std::forward<Func>(func)(sockfd, reinterpret_cast<void*>(&m_buffer_size) + m_bytes, SIZE_BYTES - m_bytes);
        if (n < 0)
        {
            perror("io(): buffer size");
            exit(EXIT_FAILURE);
        }
        m_bytes += n;
        if (m_bytes >= SIZE_BYTES)
        {
            m_buffer_size = ntohl(m_buffer_size);
            m_buffer.resize(m_buffer_size);
        }
    }
    else
    {
        uint32_t bytes = m_bytes - SIZE_BYTES; 
        int n = std::forward<Func>(func)(sockfd, m_buffer.data() + bytes, m_buffer_size - bytes);
        if (n < 0)
        {
            perror("io(): buffer");
            exit(EXIT_FAILURE);
        }
        m_bytes += n;
        bytes += n;
        if (bytes >= m_buffer_size)
        {
            m_bytes = 0;
            return true;
        }
    }
    return false;
}

}

#endif //BYTETALK_BUFFER_HPP