#include "ssl.hpp"

#include <utility>

namespace bt::net
{

void ssl_destructor(::SSL* _ssl)
{
    SSL_shutdown(_ssl);
    SSL_free(_ssl);
}

int ssl::set_socket(const basic_socket& sock)
{
    int res = SSL_set_fd(get(), sock.get());
    if (res < 0)
        return res;
    return SSL_accept(get()) == 1;
}

ssize_t ssl::read(char* buffer, size_t bytes) const
{
    return SSL_read(get(), buffer, bytes);
}

ssize_t ssl::write(const char* buffer, size_t bytes) const
{
    return SSL_write(get(), buffer, bytes);
}

ssl make_ssl(const ssl_context& ctx)
{
    auto _ssl = SSL_new(ctx.get());
    return ssl(_ssl);
}

}