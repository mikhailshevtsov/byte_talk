#ifndef BYTETALK_NET_TIMER_HPP
#define BYTETALK_NET_TIMER_HPP

#include "basic_socket.hpp"

#include <cstdint>

namespace bt::net
{

struct timer : basic_socket
{
    using basic_socket::basic_socket;
    using basic_socket::operator=;

    int set_interval(int interval);

    ssize_t read(uint64_t& expirations);
};

timer make_timer();
timer make_timer(int interval);

}

#endif //BYTETALK_NET_SOCKET_HPP
