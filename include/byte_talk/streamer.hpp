#ifndef BYTETALK_READER_HPP
#define BYTETALK_READER_HPP

#include <string_view>

namespace bt
{

class server;
class client;

class streamer
{
public:
    virtual ~streamer() = default;

    virtual bool handle(server& _server, client& _client) = 0;
};

class reader : public streamer {};

class writer : public streamer
{
public:
    virtual bool write(std::string_view buffer) = 0;
    virtual bool done() const = 0;
};

}

#endif //BYTETALK_TCPMANAGER_HPP