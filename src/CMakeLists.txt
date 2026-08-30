add_library(byte_talk
    server.cpp
    client.cpp
    buffer.cpp
    connection.cpp
    connection_pool.cpp
    signal.cpp
    timeout.cpp
)
add_subdirectory(net/)
target_link_libraries(byte_talk PRIVATE byte_talk_net)
target_include_directories(byte_talk PUBLIC ./)
target_compile_features(byte_talk_net PRIVATE cxx_std_20)