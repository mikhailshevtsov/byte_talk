#ifndef BYTETALK_CLIENT_HPP
#define BYTETALK_CLIENT_HPP

#include "connector.hpp"
#include "buffer.hpp"
#include "stream_handler.hpp"

#include <memory>
#include <any>
#include <boost/signals2.hpp>

namespace bt
{
    
struct client : public std::enable_shared_from_this<client>
{
public:
    bt::connector connector;
    std::unique_ptr<bt::reader> reader{};
    std::unique_ptr<bt::writer> writer{};
    std::any context;

    client(bt::connector&& conn) noexcept;

    boost::signals2::signal<void(server&, client&, buffer)> on_read;
    boost::signals2::signal<void(server&, client&, buffer)> on_write;
};

}

#endif //BYTETALK_CONNECTION_HPP