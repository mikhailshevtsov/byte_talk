#ifndef BYTETALK_STREAMHANDLER_HPP
#define BYTETALK_STREAMHANDLER_HPP

#include "buffer.hpp"

namespace bt
{

class server;
class client;

class stream_handler
{
public:
    virtual ~stream_handler() = default;

    virtual bool handle(server& _server, client& _client) = 0;
};

class reader : public stream_handler {};

class writer : public stream_handler
{
public:
    virtual bool write(buffer _buffer) = 0;
    virtual bool done() const = 0;
};

}

#endif //BYTETALK_STREAMHANDLER_HPP