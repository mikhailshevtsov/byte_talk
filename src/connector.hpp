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
#include <errno.h>

namespace bt
{

class connector : public socket
{
public:
    enum class status { complete, partial, error };
    
private:
    template <typename Func>
    status io_some(uint32_t& buffer_size, uint32_t& bytes, std::vector<char>& buffer, Func&& func);

public:
    using socket::socket;
    using socket::operator=;

    status read_some();
    status write_some();

    void push(std::vector<char>&& buffer);
    void push(std::string_view buffer);
    void pop();
    size_t queue_size() const noexcept;

    std::string_view read_buffer() const noexcept;
    std::string_view write_buffer() const noexcept;

    bool want_read() const noexcept;
    bool want_write() const noexcept;
    void set_want_read(bool value) noexcept;
    void set_want_write(bool value) noexcept;

private:
    static constexpr uint32_t SIZE_BYTES = 4;

    enum { read_flag = 1 << 0, write_flag = 1 << 1 };
    uint8_t m_flags = read_flag;

    std::vector<char> m_read_buffer;
    uint32_t m_read_buffer_size = 0;
    uint32_t m_read_bytes = 0;

    std::queue<std::vector<char>> m_write_buffers_queue;
    uint32_t m_write_buffer_size = 0;
    uint32_t m_write_bytes = 0;
};

template <typename Func>
connector::status connector::io_some(uint32_t& buffer_size, uint32_t& bytes, std::vector<char>& buffer, Func&& func)
{
    if (bytes < SIZE_BYTES)
    {
        int n = std::forward<Func>(func)(get(), reinterpret_cast<char*>(&buffer_size) + bytes, SIZE_BYTES - bytes);
        if (n < 0 && errno != EAGAIN && errno != EWOULDBLOCK)
            return status::error;
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
        if (n < 0 && errno != EAGAIN && errno != EWOULDBLOCK)
            return status::error;
        bytes += n;
        if (bytes >= buffer_size)
        {
            bytes = 0;
            return status::complete;
        }
    }
    return status::partial;
}

}

#endif //BYTETALK_CONNECTOR_HPP
