#ifndef BYTETALK_ACCEPTOR_HPP
#define BYTETALK_ACCEPTOR_HPP

#include "socket.hpp"
#include "connector.hpp"

namespace bt
{

struct acceptor : socket
{
    using socket::socket;
    using socket::operator=;

    bool bind(short port) const;
    bool listen(int backlog) const;

    connector accept() const;
};

}

#endif //BYTETALK_ACCEPTOR_HPP