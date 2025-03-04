#include "client.hpp"

namespace bt
{

void* client::context() const noexcept
{
    return m_context;
}

void client::set_context(void* ctx) noexcept
{
    m_context = ctx;
}

int client::id() const noexcept
{
    return m_connector.get();
}

client::client(connector&& conn, void* ctx)
    : m_connector(std::move(conn))
    , m_context{ctx}
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