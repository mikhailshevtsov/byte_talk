#include "basic_socket.hpp"

#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <utility>

namespace bt::net
{

basic_socket::basic_socket() noexcept
    : m_sockfd{-1}
{}

basic_socket::~basic_socket()
{
    close();
}

basic_socket::basic_socket(int sockfd) noexcept
    : m_sockfd{sockfd}
{}

basic_socket::basic_socket(basic_socket&& other) noexcept
    : m_sockfd{other.release()}
{}

basic_socket& basic_socket::operator=(basic_socket&& other) & noexcept
{
    swap(other);
    return *this;
}

void basic_socket::swap(basic_socket& other) noexcept
{
    std::swap(m_sockfd, other.m_sockfd);
}

basic_socket::operator bool() const noexcept
{
    return valid();
}

void basic_socket::reset() noexcept
{
    basic_socket s;
    swap(s);
}

int basic_socket::release() noexcept
{
    return std::exchange(m_sockfd, -1);
}

int basic_socket::close()
{
    int res = ::close(m_sockfd);
    m_sockfd = -1;
    return res;
}

bool basic_socket::non_blocking() const
{
    return fcntl(m_sockfd, F_GETFL, 0) & O_NONBLOCK;
}

int basic_socket::set_nonblocking(bool value)
{
    int flags = fcntl(m_sockfd, F_GETFL, 0);
    if (value)
        flags |= O_NONBLOCK;
    else
        flags &= ~O_NONBLOCK;

    return fcntl(m_sockfd, F_SETFL, flags);
}

int basic_socket::fd() const noexcept
{
    return m_sockfd;
}

bool basic_socket::valid() const noexcept
{
    return m_sockfd >= 0;
}

basic_socket make_socket()
{
    int sockfd = ::socket(AF_INET, SOCK_STREAM, 0);

    int res = -1;
    int reuse = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&reuse), sizeof(reuse));
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, reinterpret_cast<const char*>(&reuse), sizeof(reuse));

    return basic_socket{sockfd};
}

}