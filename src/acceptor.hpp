#ifndef BYTETALK_ACCEPTOR_HPP
#define BYTETALK_ACCEPTOR_HPP

#include "socket.hpp"
#include "connector.hpp"

#include <cstdint>

namespace bt
{

class acceptor : public socket
{
public:
    using socket::socket;
    using socket::operator=;

    bool bind(uint16_t port) noexcept;
    bool listen(int backlog) noexcept;

    connector accept();
};

}

#endif //BYTETALK_ACCEPTOR_HPP