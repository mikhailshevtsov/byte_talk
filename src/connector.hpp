#ifndef BYTETALK_CONNECTOR_HPP
#define BYTETALK_CONNECTOR_HPP

#include "socket.hpp"
#include "buffer.hpp"

#include <vector>

namespace bt
{

class connector : public socket
{
public:
    using socket::socket;
    using socket::operator=;

    bool read_some();
    bool write_some();

    void load(std::vector<char>&& buffer);
    void load(std::string_view buffer);

    std::string_view read_buffer() const noexcept;
    std::string_view write_buffer() const noexcept;

private:
    buffer m_read_buffer;
    buffer m_write_buffer;
};

}

#endif //BYTETALK_CONNECTOR_HPP
