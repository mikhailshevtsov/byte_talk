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

    bool bind(short port) const noexcept;
    bool listen(int backlog) const noexcept;

    connector accept() const noexcept;
};

}

#endif //BYTETALK_ACCEPTOR_HPP