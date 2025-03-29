#include <byte_talk/server.hpp>

#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

namespace bt
{

server::server(short port, size_t max_events)
    : m_port{port}
{
    m_events.resize(max_events);
}

server::~server()
{
    for (auto& _client : m_clients)
        on_close(*this, *_client);
}

int server::run()
{
    // making acceptor socket
    do m_acceptor = make_socket();
    while (!m_acceptor && (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR));
    if (!m_acceptor)
    {
        perror("make_socket");
        exit(EXIT_FAILURE);
    }

    // binding acceptor to ip-address and port
    do m_acceptor.bind(m_port);
    while (!m_acceptor && (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR));
    if (!m_acceptor)
    {
        perror("acceptor::bind");
        exit(EXIT_FAILURE);
    }
        
    // start listening
    do m_acceptor.listen(5);
    while (!m_acceptor && (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR));
    if (!m_acceptor)
    {
        perror("acceptor::listen");
        exit(EXIT_FAILURE);
    }

    // creating epoll instance
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

    // start event loop
    m_is_running = true;
    while (m_is_running)
    {
        // get events list
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

                auto _client = std::make_shared<client>(std::move(conn));
                m_clients.insert(_client);

                epoll_event e{};
                e.events = EPOLLIN | EPOLLHUP | EPOLLRDHUP | EPOLLERR;
                e.data.ptr = _client.get();

                // add new socket to epoll instance
                bool res = false;
                do res = m_epoll.add(_client->connector.get(), &e);
                while (!res && (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR));
                if (!res)
                {
                    perror("epoll::add");
                    exit(EXIT_FAILURE);
                }
                
                on_open(*this, *_client);
        
                continue;
            }

            auto _client = static_cast<client*>(m_events[i].data.ptr);

            // input event
            if (m_events[i].events & EPOLLIN && _client->reader && _client->reader->is_open())
            {
                bool res = false;
                do res = _client->reader->handle(*this, *_client);
                while (!res && (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR));
                if (!res )
                {
                    perror("connector::read");
                    exit(EXIT_FAILURE);
                }
            }
            // output event
            if (m_events[i].events & EPOLLOUT && _client->writer && _client->writer->is_open())
            {
                bool res = false;
                do res = _client->writer->handle(*this, *_client);
                while (!res && (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR));
                if (!res)
                {
                    perror("connector::write");
                    exit(EXIT_FAILURE);
                }
                if (!_client->writer->is_open())
                {
                    epoll_event e{};
                    e.events = EPOLLIN | EPOLLHUP | EPOLLRDHUP | EPOLLERR;
                    e.data.ptr = _client;

                    bool res = false;
                    do res = m_epoll.mod(_client->connector.get(), &e);
                    while (!res && (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR));
                    if (!res)
                    {
                        perror("epoll::mode");
                        exit(EXIT_FAILURE);
                    }
                }
            }
            // close event
            if (m_events[i].events & EPOLLHUP || m_events[i].events & EPOLLRDHUP || m_events[i].events & EPOLLERR)
            {
                close(*_client);
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

bool server::write_to(client& _client, buffer _buffer)
{
    if (!_client.writer->write(_buffer))
        return false;

    epoll_event e{};
    e.events = EPOLLIN | EPOLLOUT | EPOLLHUP | EPOLLRDHUP | EPOLLERR;
    e.data.ptr = &_client;

    bool res = false;
    do res = m_epoll.mod(_client.connector.get(), &e);
    while (!res && (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR));
    if (!res)
    {
        perror("epoll::mod");
        exit(EXIT_FAILURE);
    }

    return true;
}

void server::close(client& _client)
{
    on_close(*this, _client);

    bool res = false;
    do res = m_epoll.del(_client.connector.get());
    while (!res && (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR));
    if (!res)
    {
        perror("epoll::del");
        exit(EXIT_FAILURE);
    }

    m_clients.erase(_client.shared_from_this());
}

}