#include <byte_talk/common_handlers.hpp>
#include <byte_talk/client.hpp>

#include <unistd.h>
#include <arpa/inet.h>

namespace bt
{

bool common_reader::handle(server& _server, client& _client)
{
    int nbytes = 0;
    if (m_end < SIZE_BYTES)
    {
        nbytes = _client.connector.read(reinterpret_cast<char*>(&m_size) + m_end, SIZE_BYTES - m_end);
        if (nbytes < 0)
            return false;
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
        nbytes = _client.connector.read(m_buffer.data() + bytes_left, m_size - bytes_left);

        if (nbytes < 0)
            return false;
        m_end += nbytes;
        if (m_end - SIZE_BYTES >= m_size)
        {
            m_end = m_size = 0;
            _client.message_received(_server, _client, std::string(std::cbegin(m_buffer), std::cend(m_buffer)));
        }
    }
    return true;
}


bool common_writer::handle(server& _server, client& _client)
{
    int nbytes = 0;
    if (m_end < SIZE_BYTES)
    {
        nbytes = _client.connector.write(reinterpret_cast<char*>(&m_size) + m_end, SIZE_BYTES - m_end);
        if (nbytes < 0)
            return false;
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
        nbytes = _client.connector.write(m_buffer.data() + bytes_left, m_size - bytes_left);

        if (nbytes < 0)
            return false;
        m_end += nbytes;
        if (m_end - SIZE_BYTES >= m_size)
        {
            m_end = m_size = 0;
            _client.message_written(_server, _client, std::string(std::cbegin(m_buffer), std::cend(m_buffer)));
            close();
        }
    }
    return true;
}

bool common_writer::write(const std::string& _message)
{
    if (_message.empty())
        return false;
    open();
    
    m_buffer.resize(_message.size());
    std::copy(std::cbegin(_message), std::cend(_message), std::begin(m_buffer));
    m_size = htonl(static_cast<uint32_t>(_message.size()));
    m_end = 0;

    return true;
}

}
