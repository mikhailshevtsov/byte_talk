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
    static epoll create();

public:
    bool add(int sockfd, epoll_event* e);
    bool mod(int sockfd, epoll_event* e);
    bool del(int sockfd);

private:
    bool ctl(int op, int sockfd, epoll_event* e = nullptr);

public:
    int wait(epoll_event* events, int max_events);
};

}

#endif //BYTETALK_EPOLL_HPP

