#ifndef BYTETALK_WRITER_HPP
#define BYTETALK_WRITER_HPP

#include <string>

namespace bt
{

class server;
class client;

struct writer
{
    virtual void write(server& _server, client& _client) = 0;
    virtual bool load(const std::string& message) = 0;
    virtual ~writer() = default;
};

}

#endif //BYTETALK_WRITER_HPP