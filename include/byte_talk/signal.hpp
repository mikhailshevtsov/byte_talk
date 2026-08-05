#ifndef BYTETALK_SIGNAL_HPP
#define BYTETALK_SIGNAL_HPP

#include "client.hpp"

#include <vector>
#include <functional>

namespace bt
{

class signal
{
public:
    using function = std::function<void(client)>;

    signal();

    signal& operator+=(const function& slot);
    signal& operator+=(function&& slot);

    signal& remove(std::size_t index);
    const function& operator[](std::size_t index) const;
    std::size_t count() const;

    void operator()(client _client);

private:
    std::vector<std::function<void(client)>> m_slots;
};

}

#endif //BYTETALK_SIGNAL_HPP