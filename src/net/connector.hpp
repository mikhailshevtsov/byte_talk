#ifndef BYTETALK_NET_CONNECTOR_HPP
#define BYTETALK_NET_CONNECTOR_HPP

#include "socket.hpp"

#include <unistd.h>

namespace bt::net
{

struct connector : socket
{
    using socket::socket;
    using socket::operator=;

    ssize_t read(char* buffer, size_t bytes) const;
    ssize_t write(const char* buffer, size_t bytes) const;
};

}

#endif //BYTETALK_NET_CONNECTOR_HPP
