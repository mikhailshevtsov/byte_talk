#ifndef BYTETALK_READER_HPP
#define BYTETALK_READER_HPP

namespace bt
{

class server;
class client;

struct reader
{
    virtual void read(server& _server, client& _client) = 0;
    virtual ~reader() = default;
};

}

#endif //BYTETALK_READER_HPP