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

    int run();

    void write_to(std::weak_ptr<client> conn, std::string_view buf);

private:
    static void raise_error(const char* msg);

    void on_connect(std::weak_ptr<client> pclient);
    void on_read(std::weak_ptr<client> pclient, std::string_view message);
    void on_write(std::weak_ptr<client> pclient, std::string_view message);
    void on_close(std::weak_ptr<client> pclient);
    
private:
    uint16_t m_port{};
    
    epoll m_epoll{};
    std::vector<epoll_event> m_events;

    acceptor m_acceptor{}; 
    std::unordered_set<std::shared_ptr<client>> m_clients;
};

}

#endif //BYTETALK_SERVER_HPP