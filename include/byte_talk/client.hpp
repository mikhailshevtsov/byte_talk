#ifndef BYTETALK_CLIENT_HPP
#define BYTETALK_CLIENT_HPP

#include "connection.hpp"

#include <string_view>


namespace bt
{

class server;

class client
{
private:
    friend class server;
    client(bt::server* _server, connection* conn);

public:
    client();

    bool operator==(const client& other) const = default;
    operator bool() const;

    bool valid() const;
    bool alive() const;

    bt::server* server() const;
    int socket() const;
    std::string_view read() const;
    std::size_t bytes_to_send() const;
    bool sent_all() const;
    const void* data() const;
    void* data();
    void set_data(void* data);

    void set_timeout(timeout_t timeout);
    deadline_t deadline() const;

    std::size_t index() const;

    void send(std::string_view msg);
    void send();
    void close();
    
private:
    bt::server* _server{};
    connection* _conn{};
    std::size_t _age{};
};

}


#endif //BYTETALK_CLIENT_HPP