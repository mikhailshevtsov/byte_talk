#ifndef BYTETALK_NET_BASICSOCKET_HPP
#define BYTETALK_NET_BASICSOCKET_HPP

#include "unique_resource.hpp"

#include <unistd.h>

namespace bt::net
{

void basic_socket_destructor(int fd);

struct basic_socket : unique_resource<int, basic_socket_destructor, -1>
{
    using unique_resource<int, basic_socket_destructor, -1>::unique_resource;
    using unique_resource<int, basic_socket_destructor, -1>::operator=;

    bool non_blocking() const;
    int set_nonblocking(bool value);
};

// make non-blocking basic_socket
basic_socket make_socket();

}

#endif //BYTETALK_NET_BASICSOCKET_HPP