add_library(byte_talk_net
    socket.hpp socket.cpp
    acceptor.hpp acceptor.cpp
    connector.hpp connector.cpp
    epoll.hpp epoll.cpp
    errors.hpp errors.cpp
)
target_compile_features(byte_talk_net PUBLIC cxx_std_20)