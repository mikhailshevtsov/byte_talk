#include "acceptor.hpp"
#include "socket_error.hpp"

#include <cerrno>
#include <sys/socket.h>
#include <netinet/in.h>

namespace bt
{

bool acceptor::bind(short port) const
{
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int res = ::bind(fd(), (sockaddr*)&addr, sizeof(addr));
    if (res < 0)
        throw acceptor_error(fd(), errno); 
    return res >= 0;
}

bool acceptor::listen(int backlog) const
{
    int res = ::listen(fd(), backlog);
    if (res < 0)
        throw acceptor_error(fd(), errno);
    return res >= 0;
}

connector acceptor::accept() const
{
    sockaddr_in addr{};
    socklen_t addrlen{};

    int sockfd = ::accept(fd(), (sockaddr*)&addr, &addrlen);
    if (sockfd < 0)
        throw acceptor_error(fd(), errno);
    return connector{sockfd};
}

}