#ifndef BYTETALK_CONNECTOR_HPP
#define BYTETALK_CONNECTOR_HPP

#include "socket.hpp"

#include <queue>
#include <vector>
#include <string_view>
#include <cstdint>

#include <unistd.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>

namespace bt
{

class connector : public socket
{
private:
    template <typename Func>
    bool io_some(uint32_t& buffer_size, uint32_t& bytes, std::vector<char>& buffer, Func&& func);

public:
    using socket::socket;
    using socket::operator=;

    bool read_some();
    bool write_some();

    void push(std::vector<char>&& buffer);
    void push(std::string_view buffer);
    void pop();
    size_t queue_size() const noexcept;

    std::string_view read_buffer() const noexcept;
    std::string_view write_buffer() const noexcept;

private:
    static constexpr uint32_t SIZE_BYTES = 4;

    std::vector<char> m_read_buffer;
    uint32_t m_read_buffer_size = 0;
    uint32_t m_read_bytes = 0;

    std::queue<std::vector<char>> m_write_buffers_queue;
    uint32_t m_write_buffer_size = 0;
    uint32_t m_write_bytes = 0;
};

template <typename Func>
bool connector::io_some(uint32_t& buffer_size, uint32_t& bytes, std::vector<char>& buffer, Func&& func)
{
    if (bytes < SIZE_BYTES)
    {
        int n = std::forward<Func>(func)(get(), reinterpret_cast<void*>(&buffer_size) + bytes, SIZE_BYTES - bytes);
        if (n < 0)
        {
            perror("io(): buffer size");
            exit(EXIT_FAILURE);
        }
        bytes += n;
        if (bytes >= SIZE_BYTES)
        {
            buffer_size = ntohl(buffer_size);
            buffer.resize(buffer_size);
        }
    }
    else
    {
        uint32_t bytes_left = bytes - SIZE_BYTES; 
        int n = std::forward<Func>(func)(get(), buffer.data() + bytes_left, buffer_size - bytes_left);
        if (n < 0)
        {
            perror("io(): buffer");
            exit(EXIT_FAILURE);
        }
        bytes += n;
        bytes_left += n;
        if (bytes >= buffer_size)
        {
            bytes = 0;
            return true;
        }
    }
    return false;
}

}

#endif //BYTETALK_CONNECTOR_HPP
