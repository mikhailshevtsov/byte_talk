#include <byte_talk/server.hpp>

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
    if (_client.writer && _client.writer->load(message))
    {
        start_writing(_client);
        return true;
    }
    return false;
}

void server::disconnect(client& _client)
{
    client_disconnected(*this, _client);
    m_epoll.del(_client.connector());
    m_clients.erase(_client.connector().fd());
}

void server::set_writing(client& _client, bool value)
{
    uint32_t events = EVENTS;
    if (value) events |= EPOLLOUT;
    m_epoll.mod(_client.connector(), {events, &_client});
}

void server::start_writing(client& _client)
{
    set_writing(_client, true);
}

void server::stop_writing(client& _client)
{
    set_writing(_client, false);
}

int server::run()
{
    setup();

    m_is_running = true;
    while (m_is_running)
        loop();

    return 0;
}

void server::setup()
{
    try
    {
        m_acceptor = net::make_socket();
        m_acceptor.bind(m_port);
        m_acceptor.listen(5);
        m_epoll = net::epoll::create();
        m_epoll.add(m_acceptor, {EPOLLIN, &m_acceptor});
    }
    catch (const net::acceptor_error& e)
    {
        m_logger.log(e);

        switch (e.where())
        {
            case net::acceptor_error::source::bind:
                switch (e.error_code())
                {
                    case EACCES:
                        // The address is protected, and the user is not the superuser.
                        break;
                
                    case EADDRINUSE:
                        // The given address is already in use.
                        break;
                
                    case EBADF:
                        // sockfd is not a valid file descriptor.
                        break;
                
                    case EINVAL:
                        // The socket is already bound or address length/type is invalid.
                        break;
                
                    case ENOTSOCK:
                        // The file descriptor does not refer to a socket.
                        break;
                
                    case EADDRNOTAVAIL:
                        // Requested address is not available on this machine.
                        break;
                
                    case EFAULT:
                        // addr points outside the user's accessible address space.
                        break;
                
                    case ELOOP:
                        // Too many symbolic links were encountered while resolving path.
                        break;
                
                    case ENAMETOOLONG:
                        // The address path is too long.
                        break;
                
                    case ENOENT:
                        // Component in path prefix does not exist.
                        break;
                
                    case ENOMEM:
                        // Insufficient kernel memory available.
                        break;
                
                    case ENOTDIR:
                        // Component in the path prefix is not a directory.
                        break;
                
                    case EROFS:
                        // Target filesystem is read-only.
                        break;
                
                    default:
                        // Other or protocol-specific error.
                        break;
                }
                break;

            case net::acceptor_error::source::listen:
                switch (e.error_code())
                {
                    case EADDRINUSE:
                        // Another socket is already listening on the same port,
                        // or all ephemeral ports are in use when attempting to bind.
                        break;
                
                    case EBADF:
                        // The argument sockfd is not a valid file descriptor.
                        break;
                
                    case ENOTSOCK:
                        // The file descriptor sockfd does not refer to a socket.
                        break;
                
                    case EOPNOTSUPP:
                        // The socket is not of a type that supports listen().
                        break;
                
                    default:
                        // Unknown or protocol-specific error.
                        break;
                }
                break;

            default:
                break;
        }
    }
    catch (const net::epoll_error& e)
    {
        m_logger.log(e);

        switch (e.error_code())
        {
            case EINVAL:
                // size is not positive or invalid value specified in flags for epoll_create1().
                break;
        
            case EMFILE:
                // The per-process limit on the number of open file descriptors has been reached.
                break;
        
            case ENFILE:
                // The system-wide limit on the total number of open files has been reached.
                break;
        
            case ENOMEM:
                // Insufficient memory to create the kernel object.
                break;
        
            default:
                // Unknown or protocol-specific error.
                break;
        }
    }
    catch (const net::socket_error& e)
    {
        m_logger.log(e);

        switch (e.where())
        {
            case net::socket_error::source::socket:
                break;
            
            case net::socket_error::source::setsockopt:
                break;

            case net::socket_error::source::fcntl:
                break;

            case net::socket_error::source::close:
                break;

            default:
                break;
        }
    }
    catch (const std::exception& e)
    {
        m_logger.log(e);
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
                auto conn = m_acceptor.accept();
                auto _client = std::make_shared<client>(std::move(conn));
                m_clients[_client->connector().fd()] = _client;
                // add new socket to epoll instance
                m_epoll.add(_client->connector(), {EVENTS, _client.get()});
                
                client_connected(*this, *_client);

                continue;
            }

            auto _client = static_cast<client*>(m_events[i].data());
            auto events = m_events[i].events();

            // input event
            if (events & EPOLLIN && _client->reader)
            {
                _client->reader->read(*this, *_client);
            }

            // output event
            if (events & EPOLLOUT && _client->writer)
            {
                _client->writer->write(*this, *_client);
            }
            
            // close event
            if (events & EPOLLHUP || events & EPOLLRDHUP || events & EPOLLERR)
            {
                disconnect(*_client);
            }
        }
    }
    catch (const net::acceptor_error& e)
    {
        m_logger.log(e);

        switch (e.error_code())
        {
            case EAGAIN:
            //case EWOULDBLOCK:
                break;
            case EBADF:
                break;
            case ECONNABORTED:
                break;
            case EFAULT:
                break;
            case EINTR:
                break;
            case EINVAL:
                break;
            case EMFILE:
                break;
            case ENFILE:
                case ENOBUFS:
            case ENOMEM:
                break;
            case ENOTSOCK:
                break;
            case EOPNOTSUPP:
                break;
            case EPERM:
                break;
            case EPROTO:
                break;
            case ENOSR:
                break;
            case ESOCKTNOSUPPORT:
                break;
            case EPROTONOSUPPORT:
                break;
            case ETIMEDOUT:
                break;
            default:
                break;
        }
    }
    catch (const net::epoll_error& e)
    {
        m_logger.log(e);
    
        switch (e.where())
        {
            case net::epoll_error::source::wait:
                switch (e.error_code())
                {
                    case EBADF:
                        // epfd is not a valid file descriptor.
                        break;
                
                    case EFAULT:
                        // The memory area pointed to by events is not accessible with write permissions.
                        break;
                
                    case EINTR:
                        // The call was interrupted by a signal handler before any requested events occurred or the timeout expired.
                        break;
                
                    case EINVAL:
                        // epfd is not an epoll file descriptor, or maxevents is less than or equal to zero.
                        break;
                
                    default:
                        // Unknown or protocol-specific error.
                        break;
                }
                break;

            case net::epoll_error::source::ctl:
                switch (e.error_code())
                {
                    case EBADF:
                        // epfd or fd is not a valid file descriptor.
                        break;
                
                    case EEXIST:
                        // op was EPOLL_CTL_ADD, and the supplied file descriptor fd is already registered with this epoll instance.
                        break;
                
                    case EINVAL:
                        // epfd is not an epoll file descriptor, or fd is the same as epfd, or the requested operation op is not supported.
                        // or invalid event type was specified with EPOLLEXCLUSIVE in events.
                        // or op was EPOLL_CTL_MOD and events included EPOLLEXCLUSIVE.
                        // or op was EPOLL_CTL_MOD and the EPOLLEXCLUSIVE flag has previously been applied.
                        // or EPOLLEXCLUSIVE was specified and fd refers to an epoll instance.
                        break;
                
                    case ELOOP:
                        // fd refers to an epoll instance and this EPOLL_CTL_ADD operation would result in a circular loop or nesting depth greater than 5.
                        break;
                
                    case ENOENT:
                        // op was EPOLL_CTL_MOD or EPOLL_CTL_DEL, and fd is not registered with this epoll instance.
                        break;
                
                    case ENOMEM:
                        // Insufficient memory to handle the requested operation.
                        break;
                
                    case ENOSPC:
                        // The limit imposed by /proc/sys/fs/epoll/max_user_watches was encountered when trying to register a new file descriptor.
                        break;
                
                    case EPERM:
                        // The target file fd does not support epoll (e.g., refers to a regular file or directory).
                        break;
                
                    default:
                        // Unknown or protocol-specific error.
                        break;
                }
                break;
                
            default:
                break;
        }
    }
    catch (const net::connector_error& e)
    {
        m_logger.log(e);

        switch (e.where())
        {
            case net::connector_error::source::read:
                switch (e.error_code())
                {
                    case EAGAIN:
                    //case EWOULDBLOCK:
                        // The file descriptor fd refers to a socket or file marked nonblocking (O_NONBLOCK), and the read would block.
                        break;
                
                    case EBADF:
                        // fd is not a valid file descriptor or is not open for reading.
                        break;
                
                    case EFAULT:
                        // buf is outside your accessible address space.
                        break;
                
                    case EINTR:
                        // The call was interrupted by a signal before any data was read.
                        break;
                
                    case EINVAL:
                        // fd is attached to an object unsuitable for reading or the file was opened with the O_DIRECT flag and alignment is incorrect.
                        // Or fd was created via timerfd_create() and the wrong buffer size was passed to read().
                        break;
                
                    case EIO:
                        // I/O error, possibly due to process group issues or low-level I/O error.
                        break;
                
                    case EISDIR:
                        // fd refers to a directory.
                        break;
                
                    default:
                        // Unknown or protocol-specific error.
                        break;
                }
                break;

            case net::connector_error::source::write:
                switch (e.error_code())
                {
                    case EAGAIN:
                    //case EWOULDBLOCK:
                        // The file descriptor fd refers to a socket or file marked nonblocking (O_NONBLOCK), and the write would block.
                        break;
                
                    case EBADF:
                        // fd is not a valid file descriptor or is not open for writing.
                        break;
                
                    case EDESTADDRREQ:
                        // fd refers to a datagram socket for which a peer address has not been set using connect().
                        break;
                
                    case EDQUOT:
                        // The user's quota of disk blocks on the filesystem has been exhausted.
                        break;
                
                    case EFAULT:
                        // buf is outside your accessible address space.
                        break;
                
                    case EFBIG:
                        // Attempt to write a file that exceeds the maximum file size or the process's file size limit.
                        break;
                
                    case EINTR:
                        // The call was interrupted by a signal before any data was written.
                        break;
                
                    case EINVAL:
                        // fd is attached to an object unsuitable for writing, or O_DIRECT flag alignment issue.
                        break;
                
                    case EIO:
                        // A low-level I/O error occurred, possibly due to write-back issues or lost advisory lock.
                        break;
                
                    case ENOSPC:
                        // The device has no room for the data.
                        break;
                
                    case EPERM:
                        // The operation was prevented by a file seal.
                        break;
                
                    case EPIPE:
                        // fd is connected to a pipe or socket whose reading end is closed.
                        // Writing process will also receive a SIGPIPE signal.
                        break;
                
                    default:
                        // Unknown or protocol-specific error.
                        break;
                }
                break;

            default:
                break;
        }
    }
    catch (const net::socket_error& e)
    {
        m_logger.log(e);

        switch (e.where())
        {
            case net::socket_error::source::socket:
                break;
            
            case net::socket_error::source::setsockopt:
                break;

            case net::socket_error::source::fcntl:
                break;

            case net::socket_error::source::close:
                break;

            default:
                break;
        }
    }
    catch (const std::exception& e)
    {
        m_logger.log(e);
    }
}

}