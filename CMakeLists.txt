cmake_minimum_required(VERSION 3.16)
project(appServer)
set(CMAKE_CXX_STANDARD 20)

if(NOT TARGET spdlog)
    find_package(spdlog REQUIRED)
endif()

add_subdirectory(src)

add_executable(appServer main.cpp)

target_link_libraries(appServer libServer)
target_include_directories(appServer PRIVATE src)

target_link_libraries(appServer spdlog::spdlog $<$<BOOL:${MINGW}>:ws2_32>)