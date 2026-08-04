add_library(byte_talk
    server.cpp
    client.cpp
    buffer.cpp
    connection.cpp
    connection_pool.cpp
)
add_subdirectory(net)
target_link_libraries(byte_talk PRIVATE byte_talk_net)