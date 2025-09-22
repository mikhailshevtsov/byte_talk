# Minimalistic asynchronous multi-user server TCP framework

##  Description

The framework allows you to efficiently handle thousands of concurrent connections in a single thread.
Fully customizable. Different connections can use different data transfer protocols, both read and write. You can add your own protocol by overriding virtual methods of bt::reader and bt::writer classes. They are responsible for reading / writing data from the socket and notifying about receiving / sending new messages. Notifications are implemented using boost::signals. Each connection has its own signals, so you can treat each connection differently, or you can connect all the signals to the global server signals and treat all connections the same. Each connection also has a context, which can store additional information related to the client.

## Echo-server example
```
#include <byte_talk/server.hpp>
#include <byte_talk/length_prefixed_proto.hpp>
#include <iostream>

int main()
{
    static constexpr short PORT = 5555;
    bt::server echo{PORT};

    echo.client_connected.connect(
        [](bt::server& server, bt::client& client)
        {
            std::cout << "New connection on socket " << client.connector().fd() << "!\n";

            client.reader = std::make_unique<bt::length_prefixed_reader>();
            client.writer = std::make_unique<bt::length_prefixed_writer>();

            client.message_received.connect(server.message_received);
            client.message_written.connect(server.message_written);
        }
    );

    echo.request_received.connect(
        [&](bt::server& server, bt::client& client, const bt::request& request)
        {
            std::cout << "New request from socket " << client.connector().fd() << " : " << request.data << "\n";
            server.write_to(client, request.data);
        }
    );

    echo.response_sent.connect(
        [&](bt::server& server, bt::client& client, const bt::response& response)
        {
            std::cout << "Send response to socket " << client.connector().fd() << " : " << response.data << "\n";
        }
    );

    echo.client_disconnected.connect(
        [](bt::server& server, bt::client& client)
        {
            std::cout << "Close connection on socket " << client.connector().fd() << "(\n";
        }
    );

    return echo.run();
}
```