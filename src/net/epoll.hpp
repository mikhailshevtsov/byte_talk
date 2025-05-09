#ifndef BYTETALK_NET_EPOLL_HPP
#define BYTETALK_NET_EPOLL_HPP

#include "socket.hpp"

#include <sys/epoll.h>

namespace bt::net
{

struct epoll : socket
{
    struct event
    {
        constexpr event(uint32_t events = {}, void* data_ptr = {}) noexcept
        {
            ev.events = events;
            ev.data.ptr = data_ptr;
        }
        constexpr void* data() noexcept { return ev.data.ptr; }
        constexpr const void* data() const noexcept { return ev.data.ptr; }
        constexpr uint32_t events() const noexcept { return ev.events; }

        epoll_event ev{};
    };

    using socket::socket;
    using socket::operator=;
    static epoll create();

    void add(const socket& sock, event e) const;
    void mod(const socket& sock, event e) const;
    void del(const socket& sock) const;
    void ctl(int op, const socket& sock, event e = {}) const;

    int wait(event* events, int max_events) const;
};

}

#endif //BYTETALK_NET_EPOLL_HPP

