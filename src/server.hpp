#ifndef BYTETALK_SERVER_HPP
#define BYTETALK_SERVER_HPP

#include "acceptor.hpp"
#include "epoll.hpp"
#include "client.hpp"

#include <vector>
#include <string>
#include <functional>
#include <unordered_set>
#include <memory>
#include <cstdint>

#include <sys/epoll.h>

namespace bt
{

class server
{
public:
    enum class event { connect, read, write, disconnect };

public:
    server(uint16_t port, size_t max_events = 10000);
    ~server();

    int run();

    void write_to(std::shared_ptr<client> _client, std::string_view buf);
    void close(std::shared_ptr<client> _client);

    void on(event ev, const std::function<void(std::shared_ptr<client>)>& handler);
    void on(event ev, const std::function<void(std::shared_ptr<client>, std::string_view)>& handler);

private:
    static void raise_error(const char* msg);

private:
    uint16_t m_port{};
    
    epoll m_epoll{};
    std::vector<epoll_event> m_events;

    acceptor m_acceptor{}; 
    std::unordered_set<std::shared_ptr<client>> m_clients;

    std::function<void(std::shared_ptr<client>)> m_on_connect;
    std::function<void(std::shared_ptr<client>)> m_on_disconnect;
    std::function<void(std::shared_ptr<client>, std::string_view)> m_on_read;
    std::function<void(std::shared_ptr<client>, std::string_view)> m_on_write;
};

}

#endif //BYTETALK_SERVER_HPP