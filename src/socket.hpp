#ifndef BYTETALK_SOCKET_HPP
#define BYTETALK_SOCKET_HPP

namespace bt
{

// Move-only
class socket
{
public:
    socket() noexcept;
    ~socket();
    explicit socket(int sockfd) noexcept;

    socket(socket&& other) noexcept;
    socket& operator=(socket&& other) noexcept;

    socket(const socket& other) = delete;
    socket& operator=(const socket& other) = delete;

public:
    void swap(socket& other) noexcept;
    void reset();
    int release();

public:
    int get() const noexcept;
    bool is_valid() const noexcept;

    explicit operator bool() const noexcept;
    bool operator==(const socket& other) const noexcept = default;

public:
    bool set_nonblocking() noexcept;

private:
    int m_sockfd = -1;
};

socket make_socket();

struct nullsock_t
{
    operator socket() const noexcept;
};

extern nullsock_t nullsock;

}

#endif //BYTETALK_SOCKET_HPP