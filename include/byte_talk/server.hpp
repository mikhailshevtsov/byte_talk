#ifndef BYTETALK_SERVER_HPP
#define BYTETALK_SERVER_HPP

#include "net/acceptor.hpp"
#include "net/epoll.hpp"
#include "net/errors.hpp"
#include "client.hpp"

#include <boost/signals2.hpp>
#include <vector>
#include <unordered_map>
#include <memory>
#include <atomic>
#include <chrono>

namespace bt
{

class server
{
public:
    boost::signals2::signal<void(server&, client&)> client_connected;
    boost::signals2::signal<void(server&, client&)> client_disconnected;
    boost::signals2::signal<void(server&, client&, const std::string&)> message_received;
    boost::signals2::signal<void(server&, client&, const std::string&)> message_written;

public:
    static constexpr uint32_t MAX_EVENTS = 10000;

public:
    server(short port, size_t max_events = MAX_EVENTS);
    ~server();

    void stop();
    bool is_running() const noexcept;

    bool write_to(client& _client, const std::string& message);
    void disconnect(client& _client);
    void set_writing(client& _client, bool value);
    void start_writing(client& _client);
    void stop_writing(client& _client);

    int run();

private:
    void setup();
    void loop();
    
    static constexpr uint32_t EVENTS = EPOLLIN | EPOLLHUP | EPOLLRDHUP | EPOLLERR;
    static const std::chrono::zoned_time<std::chrono::milliseconds> current_date_time();

    static void log(const net::error& e) noexcept;
    static void log(const std::exception& e) noexcept;

private:
    short m_port{};
    std::atomic_bool m_is_running = false;
    
    net::epoll m_epoll{};
    std::vector<net::epoll::event> m_events;

    net::acceptor m_acceptor{}; 
    std::unordered_map<int, std::shared_ptr<client>> m_clients;
};

}

#endif //BYTETALK_SERVER_HPP