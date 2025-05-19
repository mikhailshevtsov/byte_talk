#include "socket.hpp"
#include "errors.hpp"

#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <utility>

namespace bt::net
{

socket::socket() noexcept
    : m_sockfd{-1}
{}

socket::~socket()
{
    try { close(); }
    catch (const socket_error& e) { /*skip*/ }
}

socket::socket(int sockfd) noexcept
    : m_sockfd{sockfd}
{}

socket::socket(socket&& other) noexcept
    : m_sockfd{other.release()}
{}

socket& socket::operator=(socket&& other) & noexcept
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
    socket s;
    swap(s);
}

int socket::release() noexcept
{
    return std::exchange(m_sockfd, -1);
}

void socket::close()
{
    if (is_valid() && ::close(m_sockfd) < 0)
        throw socket_error(m_sockfd, errno, socket_error::source::close);
    m_sockfd = -1;
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
        throw socket_error(sockfd, errno, socket_error::source::socket);

    int res = -1;

    int reuse = 1;
    res = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&reuse), sizeof(reuse));
    if (res < 0)
        throw socket_error(sockfd, errno, socket_error::source::setsockopt);

    res = setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, reinterpret_cast<const char*>(&reuse), sizeof(reuse));
    if (res < 0)
        throw socket_error(sockfd, errno, socket_error::source::setsockopt);

    res = fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL, 0) | O_NONBLOCK);
    if (res < 0)
        throw socket_error(sockfd, errno, socket_error::source::fcntl);

    return socket{sockfd};
}

}