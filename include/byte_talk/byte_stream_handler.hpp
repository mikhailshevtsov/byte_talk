#ifndef BYTETALK_BYTESTREAMHANDLER_HPP
#define BYTETALK_BYTESTREAMHANDLER_HPP

#include <string>

namespace bt
{

class server;
class client;

class byte_stream_handler
{
public:
    virtual ~byte_stream_handler() = default;

    virtual bool handle(server& _server, client& _client) = 0;

    bool is_open() const noexcept { return m_is_open; }
    void open() noexcept { m_is_open = true; }
    void close() noexcept { m_is_open = false; }
    void set_open(bool state) noexcept { m_is_open = state; }

private:
    bool m_is_open = true;
};

class reader : public byte_stream_handler {};

class writer : public byte_stream_handler
{
public:
    virtual bool write(const std::string& _message) = 0;
};

}

#endif //BYTETALK_BYTESTREAMHANDLER_HPP