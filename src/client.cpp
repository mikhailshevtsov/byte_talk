#include "byte_talk/client.hpp"

namespace bt
{

client::client(connector&& conn) noexcept
    : m_conn{std::move(conn)}
{}

const connector& client::get_connector() const noexcept
{
    return m_conn;
}

const std::unique_ptr<reader>& client::get_reader() const noexcept
{
    return m_reader;
}

void client::set_reader(std::unique_ptr<reader> reader) noexcept
{
    m_reader = std::move(reader);
}

const std::unique_ptr<writer>& client::get_writer() const noexcept
{
    return m_writer;
}

void client::set_writer(std::unique_ptr<writer> writer) noexcept
{
    m_writer = std::move(writer);
}

const std::any& client::get_context() const noexcept
{
    return m_context;
}

void client::set_context(std::any context)
{
    m_context = std::move(context);
}

std::weak_ptr<client> client::ptr() noexcept
{
    return weak_from_this();
}

}
