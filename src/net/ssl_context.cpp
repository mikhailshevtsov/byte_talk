#include "ssl_context.hpp"

namespace bt::net
{

void ssl_context_destructor(SSL_CTX* ssl_ctx) { SSL_CTX_free(ssl_ctx); }

ssl_context make_ssl_context(const char* cert, const char* pkey)
{
    SSL_CTX* ssl_ctx = SSL_CTX_new(TLS_server_method());

    SSL_CTX_use_certificate_file(ssl_ctx, cert, SSL_FILETYPE_PEM);
    SSL_CTX_use_PrivateKey_file(ssl_ctx, pkey, SSL_FILETYPE_PEM);

    if (SSL_CTX_check_private_key(ssl_ctx) != 1)
        return {};
    
    return ssl_context(ssl_ctx);
}

}