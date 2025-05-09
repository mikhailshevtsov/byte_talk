#ifndef BYTETALK_CLIENT_HPP
#define BYTETALK_CLIENT_HPP

#include "net/connector.hpp"
#include "reader.hpp"
#include "writer.hpp"

#include <memory>
#include <any>
#include <boost/signals2.hpp>

namespace bt
{
    
class client : public std::enable_shared_from_this<client>
{
public:
    client(net::connector&& connector) noexcept : m_connector{std::move(connector)} {}
    client(const client& other) = delete;
    client& operator=(const client& other) = delete;

    boost::signals2::signal<void(server&, client&, std::string)> message_received;
    boost::signals2::signal<void(server&, client&, std::string)> message_written;

    const net::connector& connector() const noexcept { return m_connector; }
    
    std::unique_ptr<bt::reader> reader;
    std::unique_ptr<bt::writer> writer;
    std::any context;

private:
    net::connector m_connector;
};

}

#endif //BYTETALK_CONNECTION_HPP