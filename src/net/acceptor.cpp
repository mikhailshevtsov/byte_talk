#include "acceptor.hpp"
#include "errors.hpp"

#include <sys/socket.h>
#include <netinet/in.h>

namespace bt::net
{

void acceptor::bind(short port) const
{
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int res = ::bind(fd(), (sockaddr*)&addr, sizeof(addr));
    if (res < 0)
        throw acceptor_error(fd(), errno, acceptor_error::source::bind); 
}

void acceptor::listen(int backlog) const
{
    int res = ::listen(fd(), backlog);
    if (res < 0)
        throw acceptor_error(fd(), errno, acceptor_error::source::listen);
}

connector acceptor::accept() const
{
    sockaddr_in addr{};
    socklen_t addrlen{};

    int sockfd = ::accept(fd(), (sockaddr*)&addr, &addrlen);
    if (sockfd < 0)
        throw acceptor_error(fd(), errno, acceptor_error::source::accept);
    return connector{sockfd};
}

}