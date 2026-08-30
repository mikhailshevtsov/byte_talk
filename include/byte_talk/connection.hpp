#ifndef BYTETALK_CONNECTION_HPP
#define BYTETALK_CONNECTION_HPP

#include "net/socket.hpp"
#include "net/ssl.hpp"
#include "buffer.hpp"
#include "timeout.hpp"

#include <chrono>

namespace bt
{
    
struct connection
{
    net::socket sock;
    net::ssl ssl;

    buffer rbuf;
    buffer wbuf;
    
    void* data{};

    std::size_t age = 0;
    bool alive = false;

    deadline_t deadline;

    ssize_t read();
    ssize_t write();

    void revive(net::socket _sock, net::ssl _ssl);
    void kill();
};

}

#endif //BYTETALK_CONNECTION_HPP