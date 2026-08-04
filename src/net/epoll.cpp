#include "epoll.hpp"

namespace bt::net
{

epoll epoll::create()
{
    int sockfd = epoll_create1(0);
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
    return epoll_ctl(fd(), op, sock.fd(), reinterpret_cast<epoll_event*>(&e));
}

int epoll::wait(event* events, int max_events) const
{
    return epoll_wait(fd(), reinterpret_cast<epoll_event*>(events), max_events, -1);
}

}