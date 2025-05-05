#ifndef BYTETALK_COMMONHANDLERS_HPP
#define BYTETALK_COMMONHANDLERS_HPP

#include "proto_handler.hpp"

#include <vector>
#include <cstdint>

namespace bt
{

class length_prefixed_reader : public bt::reader
{
private:
    bool read(bt::server& _server, bt::client& _client) override;

    static constexpr uint32_t SIZE_BYTES = 4;

private:
    std::vector<char> m_buffer;
    uint32_t m_size = 0;
    uint32_t m_end = 0;
};

class length_prefixed_writer : public bt::writer
{
private:
    bool write(bt::server& _server, bt::client& _client) override;
    bool load(const std::string& message) override;

    static constexpr uint32_t SIZE_BYTES = 4;

private:
    std::vector<char> m_buffer;
    uint32_t m_size = 0;
    uint32_t m_end = 0;
};

}

#endif //BYTETALK_COMMONHANDLERS_HPP