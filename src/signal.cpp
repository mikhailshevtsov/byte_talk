#include <byte_talk/signal.hpp>

namespace bt
{

signal::signal() = default;

signal& signal::operator+=(const function& slot)
{
    m_slots.push_back(slot);
    return *this;
}

signal& signal::operator+=(function&& slot)
{
    m_slots.push_back(std::move(slot));
    return *this;
}

signal& signal::remove(std::size_t index)
{
    m_slots.erase(m_slots.begin() + index);
    return *this;
}

const signal::function& signal::operator[](std::size_t index) const
{
    return m_slots.at(index);
}

std::size_t signal::count() const
{
    return m_slots.size();
}

void signal::operator()(client _client)
{
    for (auto& slot : m_slots)
        slot(_client);
}

}