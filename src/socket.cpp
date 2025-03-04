#include "socket.hpp"

#include <unistd.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <utility>

namespace bt
{

socket::socket() noexcept
    : m_sockfd{-1}
{}

socket::~socket()
{
    if (is_valid())
        close(m_sockfd);
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

bool socket::set_nonblocking() noexcept
{
    int flags = fcntl(m_sockfd, F_GETFL, 0);
    if (flags < 0)
        return false;
    return fcntl(m_sockfd, F_SETFL, flags | O_NONBLOCK) >= 0;
}

void socket::reset()
{
    socket s{};
    swap(s);
}

int socket::release()
{
    return std::exchange(m_sockfd, -1);
}

int socket::get() const noexcept
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
    if (sockfd >= 0)
        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 0, 0);
    return socket{sockfd};
}

nullsock_t::operator socket() const noexcept
{
    return {};
}

nullsock_t nullsock{};

}