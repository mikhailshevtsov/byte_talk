#ifndef BYTETALK_SERVER_HPP
#define BYTETALK_SERVER_HPP

#include "acceptor.hpp"
#include "epoll.hpp"
#include "client.hpp"
#include "buffer.hpp"

#include <boost/signals2.hpp>
#include <string_view>
#include <vector>
#include <unordered_set>
#include <memory>

namespace bt
{

class server
{
public:
    boost::signals2::signal<void(server&, client&)> on_open;
    boost::signals2::signal<void(server&, client&)> on_close;
    boost::signals2::signal<void(server&, client&, buffer)> on_read;
    boost::signals2::signal<void(server&, client&, buffer)> on_write;

public:
    server(short port, size_t max_events = 10000);
    ~server();

    int run();
    void stop();

    bool is_running() const noexcept;

    bool write_to(client& _client, buffer _buffer);
    void close(client& _client);

private:
    short m_port{};
    volatile bool m_is_running = false;
    
    epoll m_epoll{};
    std::vector<epoll_event> m_events;

    acceptor m_acceptor{}; 
    std::unordered_set<std::shared_ptr<client>> m_clients;
};

}

#endif //BYTETALK_SERVER_HPP