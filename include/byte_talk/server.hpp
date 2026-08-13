#ifndef BYTETALK_SERVER_HPP
#define BYTETALK_SERVER_HPP

#include "net/acceptor.hpp"
#include "net/epoll.hpp"
#include "signal.hpp"
#include "connection_pool.hpp"

#include <atomic>

namespace bt
{

class server
{
public:
    signal on_opened;
    signal on_closed;
    signal on_read;
    signal on_sent;

public:
    explicit server(short port, std::size_t max_conn = 10000);

    int run();

    void stop();
    bool is_running() const;

    std::size_t port() const;
    std::size_t max_connections() const;

public:
    bool setup();
    bool loop();

private:
    friend class client;
    
    void set_writing(std::size_t index, bool value);
    connection& get_connection(std::size_t index);
    void free_connection(std::size_t index);
    void close(std::size_t index);

    void print_error() const;
    
    static constexpr uint32_t EVENTS = EPOLLIN | EPOLLHUP | EPOLLRDHUP | EPOLLERR;

private:
    short m_port{};
    std::atomic_bool m_is_running = false;
    
    net::epoll m_epoll{};
    std::vector<net::epoll::event> m_events;

    net::acceptor m_acceptor{}; 
    connection_pool m_connections;
};

}

#endif //BYTETALK_SERVER_HPP