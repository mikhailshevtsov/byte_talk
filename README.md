# 🧩 byte_talk
Minimalistic asynchronous server framework


## ✨ Features
- Modern C++
- Asynchronous and single-threaded event loop
- Efficiently handles thousands of concurrent connections
- Fully customisable, allows each connection to use its own protocols and callbacks for both reading and writing


## 📝 Description
`byte_talk` allows you to efficiently handle thousands of simultaneous connections in a single thread. Each connection can use its own protocol for reading and writing data.
You can define a custom protocol by subclassing `bt::reader` and `bt::writer` classes, which handle socket I/O and emit signals on message reception or completion. Signals are implemented using `boost::signals2` and are unique per connection - allowing fine-grained or global event handling. Each connection also maintains its own context to store user-defined state related to the client.


## ⚙️ Requirements
- 🧩 **C++20** or higher  
- 🛠️ **CMake 3.28+**  
- 🪶 **boost.signals2** — header-only dependency for event handling  
- 🧱 **GCC 11+**, **Clang 14+**, or **MSVC 2022 (17.0+)**  
- 🐧 **Linux-only** (uses epoll interface)


## 🚀 Installation
This library depends on Boost.Signals2 (header-only). Please make sure Boost is installed and available in your compiler's include path:
- On Ubuntu/Debian:
```bash
sudo apt install libboost-signals-dev
```
- On Windows (vcpkg):
```bash
vcpkg install boost-signals2
```
- Using with CMake:
```cmake
# CMake
add_executable(my_app main.cpp)

# Add Boost include directory if needed
target_include_directories(my_app PRIVATE /path/to/boost)

include(FetchContent)
FetchContent_Declare(
  byte_talk
  GIT_REPOSITORY https://github.com/mikhailshevtsov/byte_talk.git
  GIT_TAG main
)
FetchContent_MakeAvailable(byte_talk)

# Link your library
target_link_libraries(my_app PRIVATE byte_talk)
```


## 💬 Echo-server example
```cpp
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


## 📜 License
MIT License © 2025 Mikhail Shevtsov
