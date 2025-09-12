add_library(byte_talk
    server.cpp
    logger.cpp logger.hpp
    length_prefixed_proto.cpp
)
add_subdirectory(net)
target_link_libraries(byte_talk PRIVATE byte_talk_net)