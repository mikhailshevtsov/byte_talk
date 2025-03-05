#include "client.hpp"

namespace bt
{

int client::id() const noexcept
{
    return m_connector.get();
}

client::client(connector&& conn)
    : m_connector(std::move(conn))
{}

connector& client::get_connector() noexcept
{
    return m_connector;
}

const connector& client::get_connector() const noexcept
{
    return m_connector;
}

}