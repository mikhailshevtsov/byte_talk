#include "epoll.hpp"

#include <unistd.h>

#include <iostream>

namespace bt
{

epoll epoll::create() noexcept
{
    return epoll{epoll_create1(0)};
}

bool epoll::add(int sockfd, epoll_event* e) const noexcept
{
    return ctl(EPOLL_CTL_ADD, sockfd, e);
}

bool epoll::mod(int sockfd, epoll_event* e) const noexcept
{
    return ctl(EPOLL_CTL_MOD, sockfd, e);
}

bool epoll::del(int sockfd) const noexcept
{
    return ctl(EPOLL_CTL_DEL, sockfd);
}

bool epoll::ctl(int op, int sockfd, epoll_event* e) const noexcept
{
    return epoll_ctl(get(), op, sockfd, e) >= 0;
}

int epoll::wait(epoll_event* events, int max_events) const noexcept
{
    return epoll_wait(get(), events, max_events, -1);
}

}