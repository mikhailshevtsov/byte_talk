#include "socket.hpp"

#include <cerrno>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <utility>

#include "socket_error.hpp"

namespace bt
{

socket::socket() noexcept
    : m_sockfd{-1}
{}

socket::~socket() noexcept
{
    if (is_valid())
        ::close(m_sockfd);
}

socket::socket(int sockfd) noexcept
    : m_sockfd{sockfd}
{}

socket::socket(socket&& other) noexcept
    : m_sockfd{other.release()}
{}

socket& socket::operator=(socket&& other) noexcept
{
    swap(other);
    return *this;
}

void socket::swap(socket& other) noexcept
{
    std::swap(m_sockfd, other.m_sockfd);
}

socket::operator bool() const noexcept
{
    return is_valid();
}

void socket::reset() noexcept
{
    socket s{};
    swap(s);
}

int socket::close() noexcept
{
    int rv = ::close(m_sockfd);
    m_sockfd = -1;
    return rv;
}

int socket::release() noexcept
{
    return std::exchange(m_sockfd, -1);
}

int socket::fd() const noexcept
{
    return m_sockfd;
}

bool socket::is_valid() const noexcept
{
    return m_sockfd >= 0;
}

socket make_socket()
{
    int sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        throw socket_error(sockfd, errno);
    return make_socket(sockfd);
}

socket make_socket(int sockfd)
{
    if (sockfd < 0)
        return {};

    int res = -1;
    res = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 0, 0);
    if (res < 0)
        throw socket_error(sockfd, errno);

    res = fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL, 0) | O_NONBLOCK);
    if (res < 0)
        throw socket_error(sockfd, errno);

    return socket{sockfd};
}

}