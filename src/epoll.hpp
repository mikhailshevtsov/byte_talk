#ifndef BYTETALK_EPOLL_HPP
#define BYTETALK_EPOLL_HPP

#include "socket.hpp"

#include <sys/epoll.h>

namespace bt
{

class epoll : public socket
{
public:
    using socket::socket;
    using socket::operator=;
    static epoll create() noexcept;

public:
    bool add(int sockfd, epoll_event* e) const noexcept;
    bool mod(int sockfd, epoll_event* e) const noexcept;
    bool del(int sockfd) const noexcept;
    bool ctl(int op, int sockfd, epoll_event* e = nullptr) const noexcept;

    int wait(epoll_event* events, int max_events) const noexcept;
};

}

#endif //BYTETALK_EPOLL_HPP

