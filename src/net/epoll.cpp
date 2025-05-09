#include "epoll.hpp"
#include "errors.hpp"

namespace bt::net
{

epoll epoll::create()
{
    int sockfd = epoll_create1(0);
    if (sockfd < 0)
        throw epoll_error(sockfd, errno, epoll_error::source::create);
    return epoll{sockfd};
}

void epoll::add(const socket& sock, event e) const
{
    ctl(EPOLL_CTL_ADD, sock, e);
}

void epoll::mod(const socket& sock, event e) const
{
    ctl(EPOLL_CTL_MOD, sock, e);
}

void epoll::del(const socket& sock) const
{
    ctl(EPOLL_CTL_DEL, sock);
}

void epoll::ctl(int op, const socket& sock, event e) const
{
    int res = epoll_ctl(fd(), op, sock.fd(), reinterpret_cast<epoll_event*>(&e));
    if (res < 0)
        throw epoll_error(fd(), errno, epoll_error::source::ctl);
}

int epoll::wait(event* events, int max_events) const
{
    int nfds = epoll_wait(fd(), reinterpret_cast<epoll_event*>(events), max_events, -1);
    if (nfds < 0)
        throw epoll_error(fd(), errno, epoll_error::source::wait);
    return nfds;
}

}