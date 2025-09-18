#ifndef BYTETALK_WRITER_HPP
#define BYTETALK_WRITER_HPP

#include "response.hpp"

namespace bt
{

class server;
class client;

struct writer
{
    virtual void write(server& _server, client& _client) = 0;
    virtual bool load(const response& _response) = 0;
    virtual ~writer() = default;
};

}

#endif //BYTETALK_WRITER_HPP