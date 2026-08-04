#ifndef BYTETALK_NET_EPOLL_HPP
#define BYTETALK_NET_EPOLL_HPP

#include "basic_socket.hpp"

#include <sys/epoll.h>
#include <cstddef>

namespace bt::net
{

struct epoll : basic_socket
{
    struct event
    {
        constexpr event(uint32_t events = {}, std::size_t index = 0) noexcept
        {
            ev.events = events;
            ev.data.u64 = index;
        }
        constexpr std::size_t index() noexcept { return ev.data.u64; }
        constexpr uint32_t events() const noexcept { return ev.events; }

        epoll_event ev{};
    };

    using basic_socket::basic_socket;
    using basic_socket::operator=;
    static epoll create();

    int add(const basic_socket& sock, event e) const;
    int mod(const basic_socket& sock, event e) const;
    int del(const basic_socket& sock) const;
    int ctl(int op, const basic_socket& sock, event e = {}) const;

    int wait(event* events, int max_events) const;
};

}

#endif //BYTETALK_NET_EPOLL_HPP

