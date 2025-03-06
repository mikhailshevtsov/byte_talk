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
#include <concepts>

#include <sys/epoll.h>

namespace bt
{

template <typename T>
concept open_close_handler = std::invocable<T, std::shared_ptr<bt::client>>;

template <typename T>
concept read_write_handler = std::invocable<T, std::shared_ptr<bt::client>, std::string_view>;

class server
{
public:
    enum class event { open, read, write, close };

public:
    server(uint16_t port, size_t max_events = 10000);
    ~server();

    int run();
    void stop();

    bool is_running() const noexcept;

    void write_to(std::shared_ptr<client> _client, std::string_view buf);
    void close(std::shared_ptr<client> _client);

    template <open_close_handler HandlerT>
    void on(event ev, HandlerT&& handler);
    
    template <read_write_handler HandlerT>
    void on(event ev, HandlerT&& handler);

private:
    static void raise_error(const char* msg);

private:
    uint16_t m_port{};
    volatile bool m_is_running = false;
    
    epoll m_epoll{};
    std::vector<epoll_event> m_events;

    acceptor m_acceptor{}; 
    std::unordered_set<std::shared_ptr<client>> m_clients;

    std::vector<std::function<void(std::shared_ptr<client>)>> m_on_open;
    std::vector<std::function<void(std::shared_ptr<client>)>> m_on_close;
    std::vector<std::function<void(std::shared_ptr<client>, std::string_view)>> m_on_read;
    std::vector<std::function<void(std::shared_ptr<client>, std::string_view)>> m_on_write;
};

template <open_close_handler HandlerT>
void server::on(event ev, HandlerT&& handler)
{
    switch (ev)
    {
        case event::open:
            m_on_open.emplace_back(std::forward<HandlerT>(handler));
            break;
        case event::close:
            m_on_close.emplace_back(std::forward<HandlerT>(handler));
            break;
    }
}

template <read_write_handler HandlerT>
void server::on(event ev, HandlerT&& handler)
{
    switch (ev)
    {
        case event::read:
            m_on_read.emplace_back(std::forward<HandlerT>(handler));
            break;
        case event::write:
            m_on_write.emplace_back(std::forward<HandlerT>(handler));
            break;
    }
}

}

#endif //BYTETALK_SERVER_HPP