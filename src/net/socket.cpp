#include "socket.hpp"

namespace bt::net
{

ssize_t socket::read(char* buffer, size_t bytes) const
{
    return ::read(get(), buffer, bytes);
}

ssize_t socket::write(const char* buffer, size_t bytes) const
{
    return ::write(get(), buffer, bytes);
}

}