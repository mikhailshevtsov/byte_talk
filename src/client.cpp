#include "byte_talk/client.hpp"

namespace bt
{

client::client(bt::connector&& conn) noexcept
    : connector{std::move(conn)}
{}

}