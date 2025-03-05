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
    for (auto& pclient : m_clients)
        close(pclient);
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

    while (true)
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
                
                m_on_connect(_client);
        
                continue;
            }

            auto _client = static_cast<client*>(m_events[i].data.ptr);
            auto& conn = _client->get_connector();
            if (m_events[i].events & EPOLLIN)
            {
                if (conn.read_some())
                    m_on_read(_client->weak_from_this(), conn.read_buffer());
            }
            if (m_events[i].events & EPOLLOUT)
            {
                if (conn.write_some())
                {
                    m_on_write(_client->weak_from_this(), conn.write_buffer());

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
                close(_client->weak_from_this());
            }
        }
    }

    return 0;
}

void server::raise_error(const char* msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}


void server::write_to(std::weak_ptr<client> cl, std::string_view buf)
{
    auto _client = cl.lock();
    if (!_client)
        return;

    _client->get_connector().push(buf);

    epoll_event e{};
    e.events = EPOLLIN | EPOLLOUT | EPOLLHUP | EPOLLRDHUP | EPOLLERR;
    e.data.ptr = _client.get();
    if (m_epoll.mod(_client->get_connector().get(), &e) < 0)
        raise_error("epoll_ctl(): mod connfd");
}

void server::close(std::weak_ptr<client> cl)
{
    auto _client = cl.lock();
    if (!_client)
        return;

    m_on_disconnect(_client);

    if (!m_epoll.del(_client->get_connector().get()))
        raise_error("epoll_ctl(): del connfd");
    m_clients.erase(_client);
}

void server::on(event ev, const std::function<void(std::weak_ptr<client>)>& handler)
{
    switch (ev)
    {
        case event::connect:
            m_on_connect = handler;
            break;
        case event::disconnect:
            m_on_disconnect = handler;
            break;
    }
}

void server::on(event ev, const std::function<void(std::weak_ptr<client>, std::string_view)>& handler)
{
    switch (ev)
    {
        case event::read:
            m_on_read = handler;
            break;
        case event::write:
            m_on_write = handler;
            break;
    }
}

}