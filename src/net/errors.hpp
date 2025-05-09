#ifndef BYTETALK_NET_ERRORS_HPP
#define BYTETALK_NET_ERRORS_HPP

#include <cerrno>
#include <cstring>
#include <exception>

namespace bt::net
{

class error : public std::exception
{
public:
    error(int sockfd, int ec) noexcept;
    
    const char* what() const noexcept override;

    int sockfd() const noexcept;
    int error_code() const noexcept;

private:
    int m_sockfd = -1;
    int m_ec = -1;
};

class socket_error : public error
{
public:
    enum class source { socket, setsockopt, fcntl };
    socket_error(int sockfd, int ec, source src) noexcept;

    source where() const noexcept;

private:
    source m_src{};
};


class epoll_error : public error
{
public:
    enum class source { create, wait, ctl };
    epoll_error(int sockfd, int ec, source src) noexcept;

    source where() const noexcept;

private:
    source m_src{};
};


class acceptor_error : public error
{
public:
    enum class source { bind, listen, accept };
    acceptor_error(int sockfd, int ec, source src) noexcept;

    source where() const noexcept;

private:
    source m_src{};
};


class connector_error : public error
{
public:
    enum class source { read, write };
    connector_error(int sockfd, int ec, source src) noexcept;

    source where() const noexcept;

private:
    source m_src{};
};


}

#endif //BYTETALK_NET_ERRORS_HPP