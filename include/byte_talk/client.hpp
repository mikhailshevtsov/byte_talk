#ifndef BYTETALK_CLIENT_HPP
#define BYTETALK_CLIENT_HPP

#include <string_view>
#include <any>


namespace bt
{

class server;

class client
{
private:
    friend class server;
    client(bt::server* server, std::size_t index);

public:
    client();
    client(const client& other);
    client& operator=(const client& other);
    client(client&& other) noexcept;
    client& operator=(client&& other) noexcept;
    ~client();

    bool operator==(const client& other) const;
    bool operator!=(const client& other) const;

    bool valid() const;
    bool alive() const;

    bt::server* server();
    const bt::server* server() const;

    std::size_t count() const;

    int socket() const;

    std::string_view read() const;
    void send(std::string_view msg);

    bool sent_all() const;

    const std::any& data() const;
    std::any& data();

    void set_data(const std::any& data);
    void set_data(std::any&& data);

    void disconnect();
    
private:
    bt::server* m_server{};
    std::size_t m_index = -1;
};

}


#endif //BYTETALK_CLIENT_HPP