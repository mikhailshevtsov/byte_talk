#include "basic_socket.hpp"

#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <utility>

namespace bt::net
{

void basic_socket_destructor(int fd) { close(fd); }

bool basic_socket::non_blocking() const
{
    return fcntl(get(), F_GETFL, 0) & O_NONBLOCK;
}

int basic_socket::set_nonblocking(bool value)
{
    int flags = fcntl(get(), F_GETFL, 0);
    if (value)
        flags |= O_NONBLOCK;
    else
        flags &= ~O_NONBLOCK;
    return fcntl(get(), F_SETFL, flags);
}

basic_socket make_socket()
{
    int sockfd = ::socket(AF_INET, SOCK_STREAM, 0);

    int reuse = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&reuse), sizeof(reuse));
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, reinterpret_cast<const char*>(&reuse), sizeof(reuse));

    return basic_socket{sockfd};
}

}