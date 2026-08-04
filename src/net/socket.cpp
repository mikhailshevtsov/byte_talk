#include "socket.hpp"

namespace bt::net
{

ssize_t socket::read(char* buffer, size_t bytes) const
{
    return ::read(fd(), buffer, bytes);
}

ssize_t socket::write(const char* buffer, size_t bytes) const
{
    return ::write(fd(), buffer, bytes);
}

}