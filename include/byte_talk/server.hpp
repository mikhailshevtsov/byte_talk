#ifndef BYTETALK_SERVER_HPP
#define BYTETALK_SERVER_HPP

#include "acceptor.hpp"
#include "epoll.hpp"
#include "client.hpp"
#include "buffer.hpp"

#include <boost/signals2.hpp>
#include <vector>
#include <unordered_set>
#include <memory>
#include <atomic>

namespace bt
{

class server
{
public:
    boost::signals2::signal<void(server&, client&)> opened;
    boost::signals2::signal<void(server&, client&)> closed;
    boost::signals2::signal<void(server&, client&, buffer)> ready_read;
    boost::signals2::signal<void(server&, client&, buffer)> ready_write;

public:
    server(uint16_t port, size_t max_events = 10000);
    ~server();

    int run();
    void stop();

    bool is_running() const noexcept;

    bool write_to(client& _client, buffer _buffer);
    void close(client& _client);

private:
    uint16_t m_port{};
    std::atomic_bool m_is_running = false;
    
    epoll m_epoll{};
    std::vector<epoll_event> m_events;

    acceptor m_acceptor{}; 
    std::unordered_set<std::shared_ptr<client>> m_clients;
};

}

#endif //BYTETALK_SERVER_HPP