#ifndef BYTETALK_NET_SSL_HPP
#define BYTETALK_NET_SSL_HPP

#include "unique_resource.hpp"
#include "basic_socket.hpp"
#include "ssl_context.hpp"

#include <openssl/ssl.h>

namespace bt::net
{

void ssl_destructor(::SSL* _ssl);

struct ssl : unique_resource<::SSL*, ssl_destructor>
{
    using unique_resource<::SSL*, ssl_destructor>::unique_resource;
    using unique_resource<::SSL*, ssl_destructor>::operator=;

    int set_socket(const basic_socket& sock);
    ssize_t read(char* buffer, size_t bytes) const;
    ssize_t write(const char* buffer, size_t bytes) const;
};

ssl make_ssl(const ssl_context& ctx);

}

#endif //BYTETALK_NET_SSL_HPP