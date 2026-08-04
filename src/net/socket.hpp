#ifndef BYTETALK_NET_SOCKET_HPP
#define BYTETALK_NET_SOCKET_HPP

#include "basic_socket.hpp"

#include <unistd.h>

namespace bt::net
{

struct socket : basic_socket
{
    using basic_socket::basic_socket;
    using basic_socket::operator=;

    ssize_t read(char* buffer, size_t bytes) const;
    ssize_t write(const char* buffer, size_t bytes) const;
};

}

#endif //BYTETALK_NET_SOCKET_HPP
