#include "errors.hpp"

namespace bt::net
{

error::error(int sockfd, int ec) noexcept
    : m_sockfd{sockfd}
    , m_ec{ec}
{}

const char* error::what() const noexcept
{
    return strerror(m_ec);
}

int error::sockfd() const noexcept
{
    return m_sockfd;
}

int error::error_code() const noexcept
{
    return m_ec;
}

const char* error::type() const noexcept
{
    return "net::error";
}

socket_error::socket_error(int sockfd, int ec, source src) noexcept
    : error(sockfd, ec)
    , m_src{src}
{}

socket_error::source socket_error::where() const noexcept
{
    return m_src;
}

const char* socket_error::type() const noexcept
{
    return "net::socket_error";
}

epoll_error::epoll_error(int sockfd, int ec, source src) noexcept
    : error(sockfd, ec)
    , m_src{src}
{}

epoll_error::source epoll_error::where() const noexcept
{
    return m_src;
}

const char* epoll_error::type() const noexcept
{
    return "net::epoll_error";
}

acceptor_error::acceptor_error(int sockfd, int ec, source src) noexcept
    : error(sockfd, ec)
    , m_src{src}
{}

acceptor_error::source acceptor_error::where() const noexcept
{
    return m_src;
}

const char* acceptor_error::type() const noexcept
{
    return "net::acceptor_error";
}

connector_error::connector_error(int sockfd, int ec, source src) noexcept
    : error(sockfd, ec)
    , m_src{src}
{}

connector_error::source connector_error::where() const noexcept
{
    return m_src;
}

const char* connector_error::type() const noexcept
{
    return "net::connector_error";
}


}