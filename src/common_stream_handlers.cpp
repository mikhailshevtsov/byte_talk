#include <byte_talk/common_stream_handlers.hpp>
#include <byte_talk/client.hpp>

#include <unistd.h>
#include <arpa/inet.h>

namespace bt
{

bool size_header_reader::handle(server& _server, client& _client)
{
    const int SIZE_BYTES = 4;
    int nbytes = 0;
    if (m_buffer.bytes < SIZE_BYTES)
    {
        nbytes = _client.connector.read(reinterpret_cast<char*>(&m_buffer.size) + m_buffer.bytes, SIZE_BYTES - m_buffer.bytes);
        if (nbytes < 0)
            return false;
        m_buffer.bytes += nbytes;
        if (m_buffer.bytes >= SIZE_BYTES)
        {
            m_buffer.size = ntohl(m_buffer.size);
            m_buffer.storage.resize(m_buffer.size);
        }
    }
    else
    {
        uint32_t bytes_left = m_buffer.bytes - SIZE_BYTES;
        nbytes = _client.connector.read(m_buffer.storage.data() + bytes_left, m_buffer.size - bytes_left);

        if (nbytes < 0)
            return false;
        m_buffer.bytes += nbytes;
        if (m_buffer.bytes - SIZE_BYTES >= m_buffer.size)
        {
            m_buffer.bytes = m_buffer.size = 0;
            _client.message_received(_server, _client, std::string(std::cbegin(m_buffer.storage), std::cend(m_buffer.storage)));
        }
    }
    return true;
}


bool size_header_writer::handle(server& _server, client& _client)
{
    const int SIZE_BYTES = 4;
    int nbytes = 0;
    if (m_buffer.bytes < SIZE_BYTES)
    {
        nbytes = _client.connector.write(reinterpret_cast<char*>(&m_buffer.size) + m_buffer.bytes, SIZE_BYTES - m_buffer.bytes);
        if (nbytes < 0)
            return false;
        m_buffer.bytes += nbytes;
        if (m_buffer.bytes >= SIZE_BYTES)
        {
            m_buffer.size = ntohl(m_buffer.size);
            m_buffer.storage.resize(m_buffer.size);
        }
    }
    else
    {
        uint32_t bytes_left = m_buffer.bytes - SIZE_BYTES;
        nbytes = _client.connector.write(m_buffer.storage.data() + bytes_left, m_buffer.size - bytes_left);

        if (nbytes < 0)
            return false;
        m_buffer.bytes += nbytes;
        if (m_buffer.bytes - SIZE_BYTES >= m_buffer.size)
        {
            m_buffer.bytes = m_buffer.size = 0;
            _client.message_written(_server, _client, std::string(std::cbegin(m_buffer.storage), std::cend(m_buffer.storage)));
            close();
        }
    }
    return true;
}

bool size_header_writer::write(const std::string& _message)
{
    if (_message.empty())
        return false;
    open();
    
    m_buffer.storage.resize(_message.size());
    std::copy(std::cbegin(_message), std::cend(_message), std::begin(m_buffer.storage));
    m_buffer.size = htonl(static_cast<uint32_t>(_message.size()));
    m_buffer.bytes = 0;

    return true;
}

}
