#ifndef BYTETALK_CONNECTIONPOOL_HPP
#define BYTETALK_CONNECTIONPOOL_HPP

#include "connection.hpp"

#include <vector>
#include <stack>


namespace bt
{
    
class connection_pool
{
public:
    explicit connection_pool(std::size_t size);

    connection* acquire();
    void release(std::size_t index);
    void release(connection* conn);

    connection& operator[](std::size_t index);
    
    std::size_t size() const;
    std::size_t free_size() const;
    std::size_t full() const;

    std::size_t index_of(connection* conn) const;

private:
    std::vector<connection> _connections;
    std::stack<std::size_t> _free_indices;
};

}

#endif //BYTETALK_CONNECTIONPOOL_HPP