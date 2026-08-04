#include "acceptor.hpp"

#include <sys/socket.h>
#include <netinet/in.h>

namespace bt::net
{

int acceptor::bind(short port) const
{
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    return ::bind(fd(), (sockaddr*)&addr, sizeof(addr));
}

int acceptor::listen(int backlog) const
{
    return ::listen(fd(), backlog);
}

socket acceptor::accept() const
{
    sockaddr_in addr{};
    socklen_t addrlen{};

    int sockfd = ::accept(fd(), (sockaddr*)&addr, &addrlen);
    return socket{sockfd};
}

}