include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.17.0
)

# For MSVC
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(byte_talk_tests byte_talk_tests.cpp)
target_link_libraries(byte_talk_tests PRIVATE byte_talk GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(byte_talk_tests)