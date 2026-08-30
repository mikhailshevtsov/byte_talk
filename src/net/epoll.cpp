#include "epoll.hpp"

namespace bt::net
{

epoll epoll::create()
{
    int sockfd = epoll_create1(EPOLL_CLOEXEC);
    return epoll{sockfd};
}

int epoll::add(const basic_socket& sock, event e) const
{
    return ctl(EPOLL_CTL_ADD, sock, e);
}

int epoll::mod(const basic_socket& sock, event e) const
{
    return ctl(EPOLL_CTL_MOD, sock, e);
}

int epoll::del(const basic_socket& sock) const
{
    return ctl(EPOLL_CTL_DEL, sock);
}

int epoll::ctl(int op, const basic_socket& sock, event e) const
{
    return epoll_ctl(get(), op, sock.get(), reinterpret_cast<epoll_event*>(&e));
}

int epoll::wait(event* events, int max_events) const
{
    return epoll_wait(get(), reinterpret_cast<epoll_event*>(events), max_events, -1);
}

}