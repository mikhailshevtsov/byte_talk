add_library(byte_talk_net
    basic_socket.cpp
    acceptor.cpp
    socket.cpp
    epoll.cpp
    ssl.cpp
    ssl_context.cpp
    timer.cpp
)

find_package(OpenSSL REQUIRED)
target_link_libraries(byte_talk_net PUBLIC OpenSSL::SSL)