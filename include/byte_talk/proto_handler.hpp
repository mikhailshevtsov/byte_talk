#ifndef BYTETALK_PROTOHANDLER_HPP
#define BYTETALK_PROTOHANDLER_HPP

#include <string>

namespace bt
{

class server;
class client;

class proto_handler
{
public:
    virtual ~proto_handler() = 0;

    bool is_open() const noexcept { return m_is_open; }
    void open() noexcept { m_is_open = true; }
    void close() noexcept { m_is_open = false; }

private:
    bool m_is_open = true;
};

struct reader : proto_handler
{
    virtual ~reader() = default;
    virtual bool read(server& _server, client& _client) = 0;
};

struct writer : proto_handler
{
    virtual ~writer() = default;
    virtual bool write(server& _server, client& _client) = 0;
    virtual bool load(const std::string& message) = 0;
};

}

#endif //BYTETALK_PROTOHANDLER_HPP