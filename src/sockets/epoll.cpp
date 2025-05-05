#include "epoll.hpp"
#include "socket_error.hpp"

#include <cerrno>

namespace bt
{

epoll epoll::create()
{
    int sockfd = epoll_create1(0);
    if (sockfd < 0)
        throw epoll_error(sockfd, errno);
    return epoll{sockfd};
}

bool epoll::add(socket& sock, event e) const
{
    return ctl(EPOLL_CTL_ADD, sock, e);
}

bool epoll::mod(socket& sock, event e) const
{
    return ctl(EPOLL_CTL_MOD, sock, e);
}

bool epoll::del(socket& sock) const
{
    return ctl(EPOLL_CTL_DEL, sock);
}

bool epoll::ctl(int op, socket& sock, event e) const
{
    int res = epoll_ctl(fd(), op, sock.fd(), reinterpret_cast<epoll_event*>(&e));
    if (res < 0)
        throw epoll_error(fd(), errno);
    return res >= 0;
}

int epoll::wait(event* events, int max_events) const
{
    int nfds = epoll_wait(fd(), reinterpret_cast<epoll_event*>(events), max_events, -1);
    if (nfds < 0)
        throw epoll_error(fd(), errno);
    return nfds;
}

}