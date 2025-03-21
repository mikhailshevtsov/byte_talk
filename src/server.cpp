#include "byte_talk/server.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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
        for (auto& handler : m_on_close)
            handler(_client);
}

int server::run()
{
    do m_acceptor = make_socket();
    while (!m_acceptor && (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR));
    if (!m_acceptor)
    {
        perror("make_socket");
        exit(EXIT_FAILURE);
    }

    do m_acceptor.bind(m_port);
    while (!m_acceptor && (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR));
    if (!m_acceptor)
    {
        perror("acceptor::bind");
        exit(EXIT_FAILURE);
    }
        
    do m_acceptor.listen(5);
    while (!m_acceptor && (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR));
    if (!m_acceptor)
    {
        perror("acceptor::listen");
        exit(EXIT_FAILURE);
    }

    // create epoll instance
    m_epoll = epoll::create();
    do m_epoll = epoll::create();
    while (!m_epoll && (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR));
    if (!m_epoll)
    {
        perror("epoll::create");
        exit(EXIT_FAILURE);
    }

    // add listening socket to epoll instance
    epoll_event e{};
    e.events = EPOLLIN;
    e.data.ptr = &m_acceptor;

    bool res = false;
    do res = m_epoll.add(m_acceptor.get(), &e);
    while (!res && (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR));
    if (!res)
    {
        perror("epoll::add");
        exit(EXIT_FAILURE);
    }

    m_is_running = true;
    while (m_is_running)
    {
        int nfds = -1;
        do nfds = m_epoll.wait(m_events.data(), m_events.size());
        while (nfds < 0 && (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR));
        if (nfds < 0)
        {
            perror("epoll::wait");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < nfds; ++i)
        {   
            // event on listening socket
            if (m_events[i].data.ptr == &m_acceptor)
            {
                connector conn;
                do conn = m_acceptor.accept();
                while (!conn && (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR));
                if (!conn)
                {
                    perror("acceptor::accept");
                    exit(EXIT_FAILURE);
                }

                if (!conn.set_nonblocking())
                {
                    perror("socket::set_nonblocking");
                    exit(EXIT_FAILURE);
                }

                auto _client = std::make_shared<client>(client(std::move(conn)));
                m_clients.insert(_client);

                epoll_event e{};
                e.events = EPOLLIN | EPOLLHUP | EPOLLRDHUP | EPOLLERR;
                e.data.ptr = _client.get();

                // add new socket to epoll instance
                bool res = false;
                do res = m_epoll.add(_client->get_connector().get(), &e);
                while (!res && (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR));
                if (!res)
                {
                    perror("epoll::add");
                    exit(EXIT_FAILURE);
                }
                
                for (auto& handler : m_on_open)
                    handler(_client);
        
                continue;
            }

            auto _client = static_cast<client*>(m_events[i].data.ptr);
            auto& conn = _client->get_connector();
            if (m_events[i].events & EPOLLIN)
            {
                bool status = false;
                bool res = false;
                do res = conn.read_some(status);
                while (!res && (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR));
                if (!res )
                {
                    perror("connector::read_some");
                    exit(EXIT_FAILURE);
                }
                if (status)
                    for (auto& handler : m_on_read)
                        handler(_client->shared_from_this(), conn.read_buffer());
            }
            if (m_events[i].events & EPOLLOUT)
            {
                bool status = false;
                bool res = false;
                do res = conn.write_some(status);
                while (!res && (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR));
                if (!res)
                {
                    perror("connector::write_some");
                    exit(EXIT_FAILURE);
                }
                if (status)
                {
                    for (auto& handler : m_on_write)
                        handler(_client->shared_from_this(), conn.write_buffer());

                    epoll_event e{};
                    e.events = EPOLLIN | EPOLLHUP | EPOLLRDHUP | EPOLLERR;
                    e.data.ptr = _client;

                    bool res = false;
                    do res = m_epoll.mod(_client->get_connector().get(), &e);
                    while (!res && (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR));
                    if (!res)
                    {
                        perror("epoll::mode");
                        exit(EXIT_FAILURE);
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

    bool res = false;
    do res = m_epoll.mod(_client->get_connector().get(), &e);
    while (!res && (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR));
    if (!res)
    {
        perror("epoll::mod");
        exit(EXIT_FAILURE);
    }
}

void server::close(std::shared_ptr<client> _client)
{
    for (auto& handler : m_on_close)
        handler(_client);

    bool res = false;
    do res = m_epoll.del(_client->get_connector().get());
    while (!res && (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR));
    if (!res)
    {
        perror("epoll::del");
        exit(EXIT_FAILURE);
    }

    auto it = m_clients.find(_client);
    (*it)->get_connector().close();
    m_clients.erase(it);
}

}