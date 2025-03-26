#include "connector.hpp"

#include <unistd.h>

namespace bt
{

int connector::read(char* buffer, int bytes) const noexcept
{
    return ::read(get(), buffer, bytes);
}

int connector::write(const char* buffer, int bytes) const noexcept
{
    return ::write(get(), buffer, bytes);
}

}