#include "server.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <iostream>

namespace bt
{

server::server(uint16_t port, size_t max_events)
    : m_port{port}
{
    m_events.resize(max_events);
}

server::~server()
{
    for (auto& _client : m_clients)
        close(_client);
}

int server::run()
{
    m_acceptor = make_socket();
    if (!m_acceptor)
    {
        perror("create acceptor");
        return -1;
    }

    if (!m_acceptor.bind(m_port))
    {
        perror("bind acceptor");
        return -1;
    }
        
    if (!m_acceptor.listen(5))
    {
        perror("listen acceptor");
        return -1;
    }

    // create epoll instance
    m_epoll = epoll::create();
    if (!m_epoll)
    {
        perror("create epoll");
        return -1;
    }

    // add listening socket to epoll instance
    epoll_event e{};
    e.events = EPOLLIN;
    e.data.ptr = &m_acceptor;
    if (!m_epoll.add(m_acceptor.get(), &e))
    {
        perror("add acceptor to epol");
        return -1;
    }

    m_is_running = true;
    while (m_is_running)
    {
        int nfds = m_epoll.wait(m_events.data(), m_events.size());
        if (nfds < 0)
        {
            perror("wait epoll");
            return -1;
        }

        for (int i = 0; i < nfds; ++i)
        {   
            // event on listening socket
            if (m_events[i].data.ptr == &m_acceptor)
            {
                connector conn = m_acceptor.accept();
                if (!conn)
                    raise_error("accept()");
                    
                if (!conn.set_nonblocking())
                    raise_error("fcntl()");

                auto _client = std::make_shared<client>(client(std::move(conn)));
                m_clients.insert(_client);

                epoll_event e{};
                e.events = EPOLLIN | EPOLLHUP | EPOLLRDHUP | EPOLLERR;
                e.data.ptr = _client.get();

                // add new socket to epoll instance
                if (!m_epoll.add(_client->get_connector().get(), &e))
                    raise_error("epoll_ctl(): add connfd");
                
                for (auto& handler : m_on_open)
                    handler(_client);
        
                continue;
            }

            auto _client = static_cast<client*>(m_events[i].data.ptr);
            auto& conn = _client->get_connector();
            if (m_events[i].events & EPOLLIN)
            {
                if (conn.read_some())
                    for (auto& handler : m_on_read)
                        handler(_client->shared_from_this(), conn.read_buffer());
            }
            if (m_events[i].events & EPOLLOUT)
            {
                if (conn.write_some())
                {
                    for (auto& handler : m_on_write)
                        handler(_client->shared_from_this(), conn.write_buffer());

                    conn.pop();
                    if (conn.queue_size() == 0)
                    {
                        epoll_event e{};
                        e.events = EPOLLIN | EPOLLHUP | EPOLLRDHUP | EPOLLERR;
                        e.data.ptr = _client;
                        if (!m_epoll.mod(_client->get_connector().get(), &e))
                            raise_error("epoll_ctl(): mod connfd");
                    }                    
                }   
            }
            if (m_events[i].events & EPOLLHUP || m_events[i].events & EPOLLRDHUP || m_events[i].events & EPOLLERR)
            {
                close(_client->shared_from_this());
            }
        }
    }

    return 0;
}

void server::stop()
{
    m_is_running = false;
}

bool server::is_running() const noexcept
{
    return m_is_running;
}

void server::write_to(std::shared_ptr<client> _client, std::string_view buf)
{
    _client->get_connector().push(buf);

    epoll_event e{};
    e.events = EPOLLIN | EPOLLOUT | EPOLLHUP | EPOLLRDHUP | EPOLLERR;
    e.data.ptr = _client.get();
    if (m_epoll.mod(_client->get_connector().get(), &e) < 0)
        raise_error("epoll_ctl(): mod connfd");
}

void server::close(std::shared_ptr<client> _client)
{
    for (auto& handler : m_on_close)
        handler(_client);

    if (!m_epoll.del(_client->get_connector().get()))
        raise_error("epoll_ctl(): del connfd");

    auto it = m_clients.find(_client);
    (*it)->get_connector().close();
    m_clients.erase(it);
}

void server::raise_error(const char* msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

}