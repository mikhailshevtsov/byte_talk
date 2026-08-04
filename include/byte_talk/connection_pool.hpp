#ifndef BYTETALK_CONNECTIONPOOL_HPP
#define BYTETALK_CONNECTIONPOOL_HPP

#include "connection.hpp"

#include <vector>
#include <queue>


namespace bt
{
    
class connection_pool
{
public:
    explicit connection_pool(std::size_t size);

    std::size_t acquire();
    void release(std::size_t index);

    connection& operator[](std::size_t index);
    
    std::size_t size() const;
    std::size_t free_size() const;
    std::size_t full() const;

private:
    std::vector<connection> m_connections;
    std::queue<std::size_t> m_free_indices;
};

}

#endif //BYTETALK_CONNECTIONPOOL_HPP