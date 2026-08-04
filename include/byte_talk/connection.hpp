#ifndef BYTETALK_CONNECTION_HPP
#define BYTETALK_CONNECTION_HPP

#include "net/socket.hpp"
#include "buffer.hpp"

#include <any>
#include <atomic>


namespace bt
{
    
struct basic_connection
{
    net::socket sock;
    buffer rbuf;
    buffer wbuf;
    std::any data;
    std::atomic_size_t count{0};
};

struct connection : basic_connection
{
    std::atomic_size_t count{0};

    connection() = default;

    connection(connection&& other) noexcept;
    connection& operator=(connection&& other) noexcept;
    
    connection(const connection&) noexcept = delete;
    connection& operator=(const connection&) noexcept = delete;

    bool read();
    bool write();

    void start(net::socket&& _sock);
    void stop();

    void inc();
    std::size_t dec();
};

}

#endif //BYTETALK_CONNECTION_HPP