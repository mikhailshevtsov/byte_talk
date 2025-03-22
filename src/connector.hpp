#ifndef BYTETALK_CONNECTOR_HPP
#define BYTETALK_CONNECTOR_HPP

#include "socket.hpp"

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
    struct buffer
    {
        std::vector<char> storage;
        uint32_t size = 0;
        uint32_t bytes = 0;
    };

    template <typename Func>
    bool io_some(buffer& buf, Func&& func, bool& is_completed);

public:
    using socket::socket;
    using socket::operator=;

    bool read_some(bool& is_completed);
    bool write_some(bool& is_completed);

    bool push(std::string_view buffer);

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

    buffer m_read_buffer;
    buffer m_write_buffer;
};

template <typename Func>
bool connector::io_some(buffer& buf, Func&& func, bool& is_completed)
{
    is_completed = false;
    int nbytes = 0;
    if (buf.bytes < SIZE_BYTES)
    {
        nbytes = std::forward<Func>(func)(get(), reinterpret_cast<char*>(&buf.size) + buf.bytes, SIZE_BYTES - buf.bytes);
        if (nbytes < 0)
            return false;
        buf.bytes += nbytes;
        if (buf.bytes >= SIZE_BYTES)
        {
            buf.size = ntohl(buf.size);
            buf.storage.resize(buf.size);
        }
    }
    else
    {
        uint32_t bytes_left = buf.bytes - SIZE_BYTES; 
        nbytes = std::forward<Func>(func)(get(), buf.storage.data() + bytes_left, buf.size - bytes_left);
        if (nbytes < 0)
            return false;
        buf.bytes += nbytes;
        if (buf.bytes - SIZE_BYTES >= buf.size)
        {
            buf.bytes = buf.size = 0;
            is_completed = true;
        }
    }
    return true;
}

}

#endif //BYTETALK_CONNECTOR_HPP
