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

                auto pclient = std::make_shared<client>(std::move(conn));
                m_clients.insert(pclient);

                epoll_event e{};
                e.events = EPOLLIN | EPOLLHUP | EPOLLRDHUP | EPOLLERR;
                e.data.ptr = pclient.get();

                // add new socket to epoll instance
                if (!m_epoll.add(pclient->get_connector().get(), &e))
                    raise_error("epoll_ctl(): add connfd");
                
                on_connect(pclient);
        
                continue;
            }

            auto pclient = static_cast<client*>(m_events[i].data.ptr);
            auto& conn = pclient->get_connector();
            if (m_events[i].events & EPOLLIN)
            {
                if (conn.read_some())
                    on_read(pclient->weak_from_this(), conn.read_buffer());
            }
            if (m_events[i].events & EPOLLOUT)
            {
                if (conn.write_some())
                {
                    on_write(pclient->weak_from_this(), conn.write_buffer());
                    conn.pop();
                    if (conn.queue_size() == 0)
                    {
                        epoll_event e{};
                        e.events = EPOLLIN | EPOLLHUP | EPOLLRDHUP | EPOLLERR;
                        e.data.ptr = pclient;
                        if (!m_epoll.mod(pclient->get_connector().get(), &e))
                            raise_error("epoll_ctl(): mod connfd");
                    }                    
                }   
            }
            if (m_events[i].events & EPOLLHUP || m_events[i].events & EPOLLRDHUP || m_events[i].events & EPOLLERR)
            {
                close(pclient->weak_from_this());
            }
        }
    }
}

void server::raise_error(const char* msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}


void server::write_to(std::weak_ptr<client> conn, std::string_view buffer)
{
    auto pconn = conn.lock();
    if (!pconn)
        return;
    pconn->get_connector().push(buffer);

    epoll_event e{};
    e.events = EPOLLIN | EPOLLOUT | EPOLLHUP | EPOLLRDHUP | EPOLLERR;
    e.data.ptr = pconn.get();
    if (m_epoll.mod(pconn->get_connector().get(), &e) < 0)
        raise_error("epoll_ctl(): mod connfd");
}

void server::close(std::weak_ptr<client> conn)
{
    auto pconn = conn.lock();
    if (!pconn)
        return;
    on_close(conn);
    if (!m_epoll.del(pconn->get_connector().get()))
        raise_error("epoll_ctl(): del connfd");
    m_clients.erase(pconn->shared_from_this());
}

void server::on_connect(std::weak_ptr<client> pclient)
{
    auto client = pclient.lock();
    std::cout << "Connect client  " << client->get_connector().get() << "\n";
}

void server::on_read(std::weak_ptr<client> pclient, std::string_view message)
{
    auto client = pclient.lock();
    std::cout << "Read from client " << client->get_connector().get() << ": ";
    if (message.size() < 100)
        std::cout << message;
    else
        std::cout << message.size() << " bytes";
    std::cout << "\n";
    write_to(client, message);
}

void server::on_write(std::weak_ptr<client> pclient, std::string_view message)
{
    auto client = pclient.lock();
    std::cout << "Write to client " << client->get_connector().get() << ": ";
    if (message.size() < 100)
        std::cout << message;
    else
        std::cout << message.size() << " bytes";
    std::cout << "\n";
}

void server::on_close(std::weak_ptr<client> pclient)
{
    auto client = pclient.lock();
    std::cout << "Disconnect client " << client->get_connector().get() << "\n";
}

}