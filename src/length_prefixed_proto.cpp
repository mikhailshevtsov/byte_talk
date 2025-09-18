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
            length_prefixed_request _request(std::string(std::cbegin(m_buffer), std::cend(m_buffer)));
            _client.request_received(_server, _client, _request);
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
            length_prefixed_response _response(std::string(std::cbegin(m_buffer), std::cend(m_buffer)));
            _client.response_sent(_server, _client, _response);
            _server.stop_writing(_client);
        }
    }
}

bool length_prefixed_writer::load(const response& _response)
{
    auto resp = dynamic_cast<const length_prefixed_response*>(&_response);

    if (!resp || resp->data.empty())
        return false;
    
    m_buffer.resize(resp->data.size());
    std::copy(std::cbegin(resp->data), std::cend(resp->data), std::begin(m_buffer));
    m_size = htonl(static_cast<uint32_t>(resp->data.size()));
    m_end = 0;

    return true;
}

}
