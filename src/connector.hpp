#ifndef BYTETALK_CONNECTOR_HPP
#define BYTETALK_CONNECTOR_HPP

#include "socket.hpp"

namespace bt
{

class connector : public socket
{
public:
    using socket::socket;
    using socket::operator=;

    int read(char* buffer, int bytes) const noexcept;
    int write(const char* buffer, int bytes) const noexcept;
};

}

#endif //BYTETALK_CONNECTOR_HPP
