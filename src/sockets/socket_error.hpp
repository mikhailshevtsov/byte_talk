#ifndef BYTETALK_SOCKETERROR_HPP
#define BYTETALK_SOCKETERROR_HPP

#include "acceptor.hpp"
#include <cstring>
#include <exception>

namespace bt
{

class socket_error : public std::exception
{
public:
    socket_error(int sockfd, int error_code) noexcept
        : m_sockfd{sockfd}
        , m_error_code{error_code}
    {}
    
    const char* what() const noexcept override
    {
        return strerror(m_error_code);
    }

    int sockfd() const noexcept
    {
        return m_sockfd;
    }

    int error_code() const noexcept
    {
        return m_error_code;
    }

private:
    int m_sockfd = -1;
    int m_error_code = -1;
};


class epoll_error : public socket_error
{
public:
    using socket_error::socket_error;
};


class acceptor_error : public socket_error
{
public:
    using socket_error::socket_error;
};


class connector_error : public socket_error
{
public:
    using socket_error::socket_error;
};


}

#endif //BYTETALK_SOCKETERROR_HPP