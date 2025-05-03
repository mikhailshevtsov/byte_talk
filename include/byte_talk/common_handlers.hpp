#ifndef BYTETALK_COMMONHANDLERS_HPP
#define BYTETALK_COMMONHANDLERS_HPP

#include "byte_stream_handler.hpp"

#include <vector>
#include <cstdint>

namespace bt
{

class common_reader : public bt::reader
{
private:
    bool handle(bt::server& _server, bt::client& _client) override;

    static constexpr uint32_t SIZE_BYTES = 4;

private:
    std::vector<char> m_buffer;
    uint32_t m_size = 0;
    uint32_t m_end = 0;
};

class common_writer : public bt::writer
{
private:
    bool handle(bt::server& _server, bt::client& _client) override;
    bool write(const std::string& _message) override;

    static constexpr uint32_t SIZE_BYTES = 4;

private:
    std::vector<char> m_buffer;
    uint32_t m_size = 0;
    uint32_t m_end = 0;
};

}

#endif //BYTETALK_COMMONHANDLERS_HPP