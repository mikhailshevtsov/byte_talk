#ifndef BYTETALK_COMMONSTREAMHANDLERS_HPP
#define BYTETALK_COMMONSTREAMHANDLERS_HPP

#include "stream_handler.hpp"

#include <vector>
#include <cstdint>

namespace bt
{

struct buffer
{
    std::vector<char> storage;
    uint32_t size = 0;
    uint32_t bytes = 0;
};

class size_header_reader : public bt::reader
{
private:
    bool handle(bt::server& _server, bt::client& _client) override;

private:
    buffer m_buffer;
};

class size_header_writer : public bt::writer
{
private:
    bool handle(bt::server& _server, bt::client& _client) override;
    bool write(const std::string& _message) override;

private:
    buffer m_buffer;
};

}

#endif //BYTETALK_COMMONSTREAMHANDLERS_HPP