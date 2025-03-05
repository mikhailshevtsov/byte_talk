#ifndef BYTETALK_USERCONNECTOR_HPP
#define BYTETALK_USERCONNECTOR_HPP

#include "connector.hpp"

#include <memory>

namespace bt
{

class client : public std::enable_shared_from_this<client>
{
public:
    friend class server;

    void* context() const noexcept;
    void set_context(void* ctx) noexcept;

    int id() const noexcept;

private:
    client(connector&& conn);

    connector& get_connector() noexcept;
    const connector& get_connector() const noexcept;

private:
    connector m_connector;
    void* m_context{};
};

}

#endif //BYTETALK_USERCONNECTOR_HPP