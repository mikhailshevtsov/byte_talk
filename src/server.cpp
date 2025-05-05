#include <byte_talk/server.hpp>
#include "sockets/socket_error.hpp"

#include <exception>
#include <iostream>

namespace bt
{

server::server(short port, size_t max_events)
    : m_port{port}
    , m_events(max_events)
{}

server::~server()
{
    for (auto& [fd, _client] : m_clients)
        client_disconnected(*this, *_client);
}

void server::stop()
{
    m_is_running = false;
}

bool server::is_running() const noexcept
{
    return m_is_running;
}

bool server::write_to(client& _client, const std::string& message)
{
    if (!_client.writer->load(message))
        return false;

    m_epoll.mod(_client.connector, epoll::event{
        EPOLLIN | EPOLLOUT | EPOLLHUP | EPOLLRDHUP | EPOLLERR,
        &_client
    });

    return true;
}

void server::close(client& _client)
{
    client_disconnected(*this, _client);

    m_epoll.del(_client.connector);

    m_clients.erase(_client.connector.fd());
}

int server::run()
{
    setup();
    // start event loop
    m_is_running = true;
    while (m_is_running)
        loop();

    return 0;
}

void server::setup()
{
    try
    {
        m_acceptor = make_socket();
        m_acceptor.bind(m_port);
        m_acceptor.listen(5);
        m_epoll = epoll::create();
        m_epoll.add(m_acceptor, epoll::event{EPOLLIN, &m_acceptor});
    }
    catch (const acceptor_error& e)
    {
    }
    catch (const epoll_error& e)
    {
    }
    catch (const socket_error& e)
    {
    }
    catch (const std::exception& e)
    {
    }
}


void server::loop()
{
    try
    {
        // get events list
        int nfds = m_epoll.wait(m_events.data(), m_events.size());
        for (int i = 0; i < nfds; ++i)
        {   
            // event on listening socket
            if (m_events[i].data() == &m_acceptor)
            {
                connector conn = m_acceptor.accept();
                auto _client = std::make_shared<client>(std::move(conn));
                m_clients[conn.fd()] = _client;

                // add new socket to epoll instance
                m_epoll.add(conn, epoll::event{
                    EPOLLIN | EPOLLHUP | EPOLLRDHUP | EPOLLERR,
                    _client.get()
                });
                
                client_connected(*this, *_client);

                continue;
            }

            auto _client = static_cast<client*>(m_events[i].data());
            auto events = m_events[i].events();

            // input event
            if (events & EPOLLIN && _client->reader && _client->reader->is_open())
            {
                _client->reader->read(*this, *_client);
            }

            // output event
            if (events & EPOLLOUT && _client->writer && _client->writer->is_open())
            {
                _client->writer->write(*this, *_client);
                if (!_client->writer->is_open())
                {
                    m_epoll.mod(_client->connector, epoll::event{
                        EPOLLIN | EPOLLHUP | EPOLLRDHUP | EPOLLERR,
                        _client
                    });
                }
            }
            
            // close event
            if (events & EPOLLHUP || events & EPOLLRDHUP || events & EPOLLERR)
            {
                close(*_client);
            }
        }
    }
    catch (const acceptor_error& e)
    {
        std::cerr << "acceptor exception on socket " << e.sockfd() << " : " << e.what() << "\n";
        if (e.error_code() != EAGAIN && e.error_code() != EWOULDBLOCK && e.error_code() != EINTR)
        {
            client& _client = *m_clients[e.sockfd()];
            close(_client);
        }
    }
    catch (const epoll_error& e)
    {
        std::cerr << "epoll exception on socket " << e.sockfd() << " : " << e.what() << "\n";
    }
    catch (const connector_error& e)
    {
        std::cerr << "connector exception on socket " << e.sockfd() << " : " << e.what() << "\n";
    }
    catch (const socket_error& e)
    {
        std::cerr << "socket exception on socket " << e.sockfd() << " : " << e.what() << "\n";
    }
    catch (const std::exception& e)
    {
        std::cerr << "unknown exception " << e.what() << "\n";
    }
}

}