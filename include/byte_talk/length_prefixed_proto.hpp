#ifndef BYTETALK_LENGTHPREFIXPROTO_HPP
#define BYTETALK_LENGTHPREFIXPROTO_HPP

#include "reader.hpp"
#include "writer.hpp"

#include <vector>
#include <cstdint>

namespace bt
{

class length_prefixed_reader : public reader
{
private:
    void read(server& _server, client& _client) override;

    static constexpr uint32_t SIZE_BYTES = 4;

private:
    std::vector<char> m_buffer;
    uint32_t m_size = 0;
    uint32_t m_end = 0;
};

class length_prefixed_writer : public writer
{
private:
    void write(server& _server, client& _client) override;
    bool load(const std::string& message) override;

    static constexpr uint32_t SIZE_BYTES = 4;

private:
    std::vector<char> m_buffer;
    uint32_t m_size = 0;
    uint32_t m_end = 0;
};

}

#endif //BYTETALK_LENGTHPREFIXPROTO_HPP