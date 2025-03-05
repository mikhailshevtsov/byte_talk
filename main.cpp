#include <iostream>

#include "server.hpp"

int main()
{
    bt::server server{15000};

    server.on(
        bt::server::event::connect,
        [&](std::weak_ptr<bt::client> cl)
        {
            auto client = cl.lock();
            std::cout << "Connect client  " << client->id() << "\n";
        }
    );

    server.on(
        bt::server::event::read,
        [&](std::weak_ptr<bt::client> cl, std::string_view message)
        {
            auto client = cl.lock();
            std::cout << "Read from client " << client->id() << ": " << message << "\n";
            server.write_to(client, message);
        }
    );

    server.on(
        bt::server::event::write,
        [&](std::weak_ptr<bt::client> cl, std::string_view message)
        {
            auto client = cl.lock();
            std::cout << "Write to client " << client->id() << ": " << message << "\n";
        }
    );

    server.on(
        bt::server::event::disconnect,
        [&](std::weak_ptr<bt::client> cl)
        {
            auto client = cl.lock();
            std::cout << "Disconnect client " << client->id() << "\n";
        }
    );

    server.run();

    return 0;
}