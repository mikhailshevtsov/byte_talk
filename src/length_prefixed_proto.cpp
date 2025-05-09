#include <byte_talk/length_prefixed_proto.hpp>
#include <byte_talk/server.hpp>
#include <byte_talk/client.hpp>

#include <arpa/inet.h>

namespace bt
{

void length_prefixed_reader::read(server& _server, client& _client)
{
    if (m_end < SIZE_BYTES)
    {
        int nbytes = _client.connector().read(reinterpret_cast<char*>(&m_size) + m_end, SIZE_BYTES - m_end);
        m_end += nbytes;
        if (m_end >= SIZE_BYTES)
        {
            m_size = ntohl(m_size);
            m_buffer.resize(m_size);
        }
    }
    else
    {
        uint32_t bytes_left = m_end - SIZE_BYTES;
        int nbytes = _client.connector().read(m_buffer.data() + bytes_left, m_size - bytes_left);
        m_end += nbytes;
        if (m_end - SIZE_BYTES >= m_size)
        {
            m_end = m_size = 0;
            _client.message_received(_server, _client, std::string(std::cbegin(m_buffer), std::cend(m_buffer)));
        }
    }
}

void length_prefixed_writer::write(server& _server, client& _client)
{
    if (m_end < SIZE_BYTES)
    {
        int nbytes = _client.connector().write(reinterpret_cast<const char*>(&m_size) + m_end, SIZE_BYTES - m_end);
        m_end += nbytes;
        if (m_end >= SIZE_BYTES)
        {
            m_size = ntohl(m_size);
            m_buffer.resize(m_size);
        }
    }
    else
    {
        uint32_t bytes_left = m_end - SIZE_BYTES;
        int nbytes = _client.connector().write(m_buffer.data() + bytes_left, m_size - bytes_left);
        m_end += nbytes;
        if (m_end - SIZE_BYTES >= m_size)
        {
            m_end = m_size = 0;
            _client.message_written(_server, _client, std::string(std::cbegin(m_buffer), std::cend(m_buffer)));
            _server.stop_writing(_client);
        }
    }
}

bool length_prefixed_writer::load(const std::string& message)
{
    if (message.empty())
        return false;
    
    m_buffer.resize(message.size());
    std::copy(std::cbegin(message), std::cend(message), std::begin(m_buffer));
    m_size = htonl(static_cast<uint32_t>(message.size()));
    m_end = 0;

    return true;
}

}
