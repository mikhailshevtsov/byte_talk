#ifndef BYTETALK_SERVER_HPP
#define BYTETALK_SERVER_HPP

#include "sockets/acceptor.hpp"
#include "sockets/epoll.hpp"
#include "client.hpp"

#include <boost/signals2.hpp>
#include <vector>
#include <unordered_map>
#include <memory>
#include <atomic>

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
    void close(client& _client);

    int run();

private:
    void setup();
    void loop();

private:
    short m_port{};
    std::atomic_bool m_is_running = false;
    
    epoll m_epoll{};
    std::vector<epoll::event> m_events;

    acceptor m_acceptor{}; 
    std::unordered_map<int, std::shared_ptr<client>> m_clients;
};

}

#endif //BYTETALK_SERVER_HPP