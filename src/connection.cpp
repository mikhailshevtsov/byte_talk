#include <byte_talk/connection.hpp>

#include <cerrno>

namespace bt
{

ssize_t connection::read()
{
    if (!sock)
        return -1;
    std::size_t _read = 0;
    while (true)
    {
        if (rbuf.full())
            rbuf.extend(rbuf.size());

        auto [data, size] = std::make_pair(rbuf.last(), rbuf.bytes_left());    
        ssize_t n = ssl ? ssl.read(data, size) : sock.read(data, size);

        if (n > 0)
        {
            rbuf.add_bytes(n);
            _read += n;
        }
        else if (errno == EAGAIN || errno == EWOULDBLOCK)
            break;
        else if (errno == EINTR)
            continue;
        else
            return n;
    }
    return _read;
}

ssize_t connection::write()
{
    if (!sock)
        return -1;
    std::size_t total = wbuf.size();
    std::size_t sent = 0;
    while (sent < total)
    {
        auto [data, size] = std::make_pair(wbuf.at(sent), total - sent);
        ssize_t n = ssl ? ssl.write(data, size) : sock.write(data, size);

        if (n > 0)
            sent += n;
        else if (errno == EAGAIN || errno == EWOULDBLOCK)
            break;
        else if (errno == EINTR)
            continue;
        else
            return n;
    }
    wbuf.pop(sent);
    return sent;
}

void connection::revive(net::socket _sock, net::ssl _ssl)
{
    sock = std::move(_sock);
    ssl = std::move(_ssl);
    rbuf.clear();
    wbuf.clear();
    data = nullptr;
    alive = true;
    deadline = {};
}

void connection::kill()
{
    sock.close();
    ssl.close();
    rbuf.clear();
    wbuf.clear();
    data = nullptr;
    alive = false;
    ++age;
    deadline = {};
}

}