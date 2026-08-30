#ifndef BYTETALK_NET_SSLCONTEXT_HPP
#define BYTETALK_NET_SSLCONTEXT_HPP

#include "unique_resource.hpp"

#include <openssl/ssl.h>

namespace bt::net
{

void ssl_context_destructor(SSL_CTX* ssl_ctx);

struct ssl_context : unique_resource<SSL_CTX*, ssl_context_destructor>
{
    using unique_resource<SSL_CTX*, ssl_context_destructor>::unique_resource;
    using unique_resource<SSL_CTX*, ssl_context_destructor>::operator=;
};

ssl_context make_ssl_context(const char* cert, const char* pkey);

}

#endif //BYTETALK_NET_SSLCONTEXT_HPP