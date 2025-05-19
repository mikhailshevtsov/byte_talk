#ifndef BYTETALK_NET_SOCKET_HPP
#define BYTETALK_NET_SOCKET_HPP

namespace bt::net
{

class socket
{
public:
    socket() noexcept;
    ~socket();
    explicit socket(int sockfd) noexcept;

    socket(socket&& other) noexcept;
    socket& operator=(socket&& other) & noexcept;

    socket(const socket& other) = delete;
    socket& operator=(const socket& other) = delete;

public:
    void swap(socket& other) noexcept;
    void reset() noexcept;
    int release() noexcept;

    void close();

public:
    int fd() const noexcept;
    bool is_valid() const noexcept;

    explicit operator bool() const noexcept;
    bool operator==(const socket& other) const noexcept = default;

private:
    int m_sockfd = -1;
};

// make non-blocking socket
socket make_socket();

}

#endif //BYTETALK_NET_SOCKET_HPP