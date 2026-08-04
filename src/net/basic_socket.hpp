#ifndef BYTETALK_NET_BASICSOCKET_HPP
#define BYTETALK_NET_BASICSOCKET_HPP

namespace bt::net
{

class basic_socket
{
public:
    basic_socket() noexcept;
    ~basic_socket();
    explicit basic_socket(int sockfd) noexcept;

    basic_socket(basic_socket&& other) noexcept;
    basic_socket& operator=(basic_socket&& other) & noexcept;

    basic_socket(const basic_socket& other) = delete;
    basic_socket& operator=(const basic_socket& other) = delete;

public:
    void swap(basic_socket& other) noexcept;
    void reset() noexcept;
    int release() noexcept;

    int close();

    bool non_blocking() const;
    int set_nonblocking(bool value);

public:
    int fd() const noexcept;
    bool valid() const noexcept;

    explicit operator bool() const noexcept;
    bool operator==(const basic_socket& other) const noexcept = default;

private:
    int m_sockfd = -1;
};

// make non-blocking basic_socket
basic_socket make_socket();

}

#endif //BYTETALK_NET_BASICSOCKET_HPP