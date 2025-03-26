#ifndef BYTETALK_CLIENT_HPP
#define BYTETALK_CLIENT_HPP

#include "connector.hpp"
#include "byte_talk/streamer.hpp"

#include <memory>
#include <any>
#include <boost/signals2.hpp>

namespace bt
{
    

class client : public std::enable_shared_from_this<client>
{
public:
    client(connector&& conn) noexcept;

    const connector& get_connector() const noexcept;

    const std::unique_ptr<reader>& get_reader() const noexcept;
    void set_reader(std::unique_ptr<reader> reader) noexcept;

    const std::unique_ptr<writer>& get_writer() const noexcept;
    void set_writer(std::unique_ptr<writer> writer) noexcept;

    const std::any& get_context() const noexcept;

    void set_context(std::any context);

    boost::signals2::signal<void(server&, client&, std::string_view)> on_read;
    boost::signals2::signal<void(server&, client&, std::string_view)> on_write;

    std::weak_ptr<client> ptr() noexcept;

private:
    connector m_conn;
    std::unique_ptr<reader> m_reader{};
    std::unique_ptr<writer> m_writer{};
    std::any m_context;
};

}

#endif //BYTETALK_CONNECTION_HPP