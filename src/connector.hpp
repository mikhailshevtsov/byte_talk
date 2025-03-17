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
private:
    template <typename Func>
    bool io_some(uint32_t& buffer_size, uint32_t& bytes, std::vector<char>& buffer, Func&& func, bool& is_completed);

public:
    using socket::socket;
    using socket::operator=;

    bool read_some(bool& is_completed);
    bool write_some(bool& is_completed);

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
bool connector::io_some(uint32_t& buffer_size, uint32_t& bytes, std::vector<char>& buffer, Func&& func, bool& is_completed)
{
    is_completed = false;
    int nbytes = 0;
    if (bytes < SIZE_BYTES)
    {
        nbytes = std::forward<Func>(func)(get(), reinterpret_cast<char*>(&buffer_size) + bytes, SIZE_BYTES - bytes);
        if (nbytes < 0)
            return false;
        bytes += nbytes;
        if (bytes >= SIZE_BYTES)
        {
            buffer_size = ntohl(buffer_size);
            buffer.resize(buffer_size);
        }
    }
    else
    {
        uint32_t bytes_left = bytes - SIZE_BYTES; 
        nbytes = std::forward<Func>(func)(get(), buffer.data() + bytes_left, buffer_size - bytes_left);
        if (nbytes < 0)
            return false;
        bytes += nbytes;
        if (bytes >= buffer_size)
        {
            bytes = 0;
            is_completed = true;
        }
    }
    return true;
}

}

#endif //BYTETALK_CONNECTOR_HPP
