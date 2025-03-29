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

    bool is_open() const noexcept { return m_is_open; }
    void open() noexcept { m_is_open = true; }
    void close() noexcept { m_is_open = false; }
    void set_open(bool state) noexcept { m_is_open = state; }

private:
    bool m_is_open = true;
};

class reader : public stream_handler {};

class writer : public stream_handler
{
public:
    virtual bool write(buffer _buffer) = 0;
};

}

#endif //BYTETALK_STREAMHANDLER_HPP