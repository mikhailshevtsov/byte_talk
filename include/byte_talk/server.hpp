#ifndef BYTETALK_SERVER_HPP
#define BYTETALK_SERVER_HPP

#include "net/acceptor.hpp"
#include "net/epoll.hpp"
#include "net/ssl_context.hpp"
#include "net/timer.hpp"
#include "config.hpp"
#include "signal.hpp"
#include "connection_pool.hpp"
#include "timeout.hpp"

#include <queue>

namespace bt
{

class server
{
public:
    signal on_opened;
    signal on_closed;
    signal on_read;
    signal on_sent;
    signal on_expired;

public:
    explicit server(const config& cfg);
    const bt::config& config() const;

public:
    int setup();
    int loop();
    int run();

private:
    int check_timeouts();
    
    int open();
    int close(connection* conn);
    ssize_t read(connection* conn);
    ssize_t send(connection* conn);

    void set_timeout(connection* conn, timeout_t timeout);

    void set_writing(connection* conn, bool value);
    void print_error() const;
    
    static constexpr uint32_t EVENTS = EPOLLIN | EPOLLHUP | EPOLLRDHUP | EPOLLERR;

    bool is_timer_fired(const net::epoll::event& event) const;
    bool is_opened(const net::epoll::event& event) const;
    bool is_closed(const net::epoll::event& event) const;
    bool is_read(const net::epoll::event& event) const;
    bool is_sent(const net::epoll::event& event) const;

public:
    void send_to(client _client, std::string_view msg);
    void send_to(client _client);
    void close(client _client);
    void set_timeout(client _client, timeout_t timeout);

    std::size_t index_of(client _client) const;

private:
    bt::config _cfg;

    net::epoll _epoll{};
    std::vector<net::epoll::event> _events;
    net::acceptor _acceptor{}; 
    connection_pool _connections;
    net::ssl_context _ssl_ctx{};
    net::timer _timer{};

    struct timeout_manager
    {
        connection* conn;
        std::size_t age{};
        deadline_t deadline;
    };

    struct timeout_manager_compare
    {
        bool operator()(const timeout_manager& tm1, const timeout_manager& tm2) const
        {
            return tm1.deadline > tm2.deadline;
        }
    };

    std::priority_queue<timeout_manager, std::vector<timeout_manager>, timeout_manager_compare> _timeouts;
    int _interval = 1; //seconds
};

}

#endif //BYTETALK_SERVER_HPP