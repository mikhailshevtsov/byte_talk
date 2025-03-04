#include "acceptor.hpp"

#include <sys/socket.h>
#include <netinet/in.h>

#include <utility>

namespace bt
{

bool acceptor::bind(uint16_t port) noexcept
{
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    return ::bind(get(), (sockaddr*)&addr, sizeof(addr)) >= 0;
}

bool acceptor::listen(int backlog) noexcept
{
    return ::listen(get(), backlog) >= 0;
}

connector acceptor::accept()
{
    sockaddr_in addr{};
    socklen_t addrlen{};
    return connector{::accept(get(), (sockaddr*)&addr, &addrlen)};
}

}