#include <iostream>

#include "server.hpp"

int main()
{
    bt::server server{15000};

    server.on(
        bt::server::event::open,
        [&](std::shared_ptr<bt::client> client)
        {
            std::cout << "Connect client  " << client->id() << "\n";
        }
    );

    server.on(
        bt::server::event::read,
        [&](std::shared_ptr<bt::client> client, std::string_view message)
        {
            std::cout << "Read from client " << client->id() << ": " << message << "\n";
            server.write_to(client, message);
        }
    );

    server.on(
        bt::server::event::write,
        [&](std::shared_ptr<bt::client> client, std::string_view message)
        {
            std::cout << "Write to client " << client->id() << ": " << message << "\n";
        }
    );

    server.on(
        bt::server::event::close,
        [&](std::shared_ptr<bt::client> client)
        {
            std::cout << "Disconnect client " << client->id() << "\n";
        }
    );

    server.run();

    return 0;
}