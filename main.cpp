#include <iostream>

#include "server.hpp"

int main()
{
    bt::server server{15000};
    server.run();

    // server.on(
    //     bt::server::event::connect,
    //     [&](std::weak_ptr<bt::client> client)
    //     {
    //          server.write_to(client, "Hello, Client!");
    //     }
    // );

    // server.on(
    //     bt::server::event::read,
    //     [&](std::weak_ptr<bt::client> client, std::string_view message)
    //     {
    //          server.write_to(client, message);
    //     }
    // );

    // server.on(
    //     bt::server::event::write,
    //     [&](std::weak_ptr<bt::client> client, std::string_view message)
    //     {
    //          std::cout << message << " sent!\n";
    //     }
    // );

    // server.on(
    //     bt::server::event::disconnect,
    //     [&](std::weak_ptr<bt::client> client, std::string_view message)
    //     {
    //          std::cout << message << " sent!\n";
    //     }
    // );
}