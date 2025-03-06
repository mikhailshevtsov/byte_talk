#ifndef BYTETALK_CLIENT_HPP
#define BYTETALK_CLIENT_HPP

#include "connector.hpp"

#include <memory>

namespace bt
{

class client : public std::enable_shared_from_this<client>
{
public:
    friend class server;

    template <typename T>
    T* context() const noexcept;

    template <typename T>
    void set_context(T* ctx) noexcept;

    int id() const noexcept;

private:
    client(connector&& conn);

    connector& get_connector() noexcept;
    const connector& get_connector() const noexcept;

private:
    connector m_connector;
    void* m_context = nullptr;
    const std::type_info& (*m_context_type)() = []() -> const std::type_info& { return typeid(void); };
};

template <typename T>
T* client::context() const noexcept
{
    return typeid(T) == m_context_type() ? static_cast<T*>(m_context) : nullptr;
}

template <typename T>
void client::set_context(T* ctx) noexcept
{
    m_context = ctx;
    m_context_type = []() -> const std::type_info& { return typeid(T); };
}

}

#endif //BYTETALK_CLIENT_HPP