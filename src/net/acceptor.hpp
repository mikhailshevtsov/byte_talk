#ifndef BYTETALK_NET_ACCEPTOR_HPP
#define BYTETALK_NET_ACCEPTOR_HPP

#include "basic_socket.hpp"
#include "socket.hpp"

namespace bt::net
{

struct acceptor : basic_socket
{
    using basic_socket::basic_socket;
    using basic_socket::operator=;

    int bind(short port) const;
    int listen(int backlog = 5) const;

    socket accept() const;
};

}

#endif //BYTETALK_NET_ACCEPTOR_HPP