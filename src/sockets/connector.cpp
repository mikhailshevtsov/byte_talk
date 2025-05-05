#include "connector.hpp"
#include "socket_error.hpp"

#include <cerrno>

namespace bt
{

ssize_t connector::read(char* buffer, size_t bytes) const
{
    ssize_t n = ::read(fd(), buffer, bytes);
    if (n < 0)
        throw connector_error(fd(), errno);
    return n;
}

ssize_t connector::write(const char* buffer, size_t bytes) const
{
    ssize_t n = ::write(fd(), buffer, bytes);
    if (n < 0)
        throw connector_error(fd(), errno);
    return n;
}

}