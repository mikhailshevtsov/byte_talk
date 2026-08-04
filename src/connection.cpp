#include <byte_talk/connection.hpp>

namespace bt
{

connection::connection(connection&& other) noexcept
    : count(other.count.load())
{}

connection& connection::operator=(connection&& other) noexcept
{
    connection tmp(std::move(other));
    std::swap(*this, tmp);
    return *this;
}

bool connection::read()
{
    while (true)
    {
        if (rbuf.full())
            rbuf.extend(rbuf.size());

        ssize_t n = sock.read(rbuf.last(), rbuf.bytes_left());

        if (n > 0)
            rbuf.add_bytes(n);
        else if (errno == EAGAIN || errno == EWOULDBLOCK)
            break;
        else if (errno == EINTR)
            continue;
        else
            return false;
    }
    return true;
}

bool connection::write()
{
    std::size_t total = wbuf.size();
    std::size_t sent = 0;

    while (sent < total)
    {
        ssize_t n = sock.write(wbuf.at(sent), total - sent);

        if (n > 0)
            sent += n;
        else if (errno == EAGAIN || errno == EWOULDBLOCK)
            break;
        else if (errno == EINTR)
            continue;
        else
            return false;
    }
    wbuf.pop(sent);
    return true;
}

void connection::start(net::socket&& _sock)
{
    sock = std::move(_sock);
}

void connection::stop()
{
    sock.close();
}

void connection::inc()
{
    count.fetch_add(1, std::memory_order_relaxed);
}

std::size_t connection::dec()
{
    return count.fetch_sub(1, std::memory_order_acq_rel);
}

}