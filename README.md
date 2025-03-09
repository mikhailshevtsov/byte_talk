# Minimalistic asynchronous multi-user server TCP framework

## Simple and convenient abstraction over the epoll event loop

The project is essentially a simple and convenient abstraction over the epoll event loop. Thus, a byte_talk will be able to efficiently handle thousands of connections per second. Interaction with clients is based on a simple event system. The framework is object-oriented and easy to use.

## Echo-server example
```
#include <byte_talk/server.hpp>

int main()
{
    bt::server echo{15000};

    echo.on(
        bt::server::event::read,
        [&](std::shared_ptr<bt::client> client, std::string_view message)
        {
            echo.write_to(client, message);
        }
    );

    return echo.run();
}
```