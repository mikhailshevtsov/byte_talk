#ifndef BYTETALK_EPOLL_HPP
#define BYTETALK_EPOLL_HPP

#include "socket.hpp"

#include <sys/epoll.h>

namespace bt
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

    bool add(socket& sock, event e) const;
    bool mod(socket& sock, event e) const;
    bool del(socket& sock) const;
    bool ctl(int op, socket& sock, event e = {}) const;

    int wait(event* events, int max_events) const;
};

}

#endif //BYTETALK_EPOLL_HPP

