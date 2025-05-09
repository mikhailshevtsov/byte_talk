#ifndef BYTETALK_NET_ACCEPTOR_HPP
#define BYTETALK_NET_ACCEPTOR_HPP

#include "socket.hpp"
#include "connector.hpp"

namespace bt::net
{

struct acceptor : socket
{
    using socket::socket;
    using socket::operator=;

    void bind(short port) const;
    void listen(int backlog) const;

    connector accept() const;
};

}

#endif //BYTETALK_NET_ACCEPTOR_HPP