set(LIBRARY_NAME byte_talk_sockets)

add_library(${LIBRARY_NAME})
target_sources(${LIBRARY_NAME}
    PRIVATE
    socket.hpp socket.cpp
    acceptor.hpp acceptor.cpp
    connector.hpp connector.cpp
    epoll.hpp epoll.cpp
)

target_compile_features(${LIBRARY_NAME} PRIVATE cxx_std_20)