#include "acceptor.hpp"

#include <sys/socket.h>
#include <netinet/in.h>

#include <utility>

namespace bt
{

bool acceptor::bind(short port) const noexcept
{
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    return ::bind(get(), (sockaddr*)&addr, sizeof(addr)) >= 0;
}

bool acceptor::listen(int backlog) const noexcept
{
    return ::listen(get(), backlog) >= 0;
}

connector acceptor::accept() const noexcept
{
    sockaddr_in addr{};
    socklen_t addrlen{};
    return connector{::accept(get(), (sockaddr*)&addr, &addrlen)};
}

}