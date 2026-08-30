cmake_minimum_required(VERSION 3.20)
project(
    byte_talk
    VERSION 1.0.0
    LANGUAGES CXX
)

add_subdirectory(src/)
add_library(byte_talk::byte_talk ALIAS byte_talk)

target_include_directories(
    byte_talk
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_compile_features(byte_talk PUBLIC cxx_std_20)


# --- EXAMPLES ---

option(BYTE_TALK_BUILD_EXAMPLES "Build byte_talk examples" ${PROJECT_IS_TOP_LEVEL})

if(BYTE_TALK_BUILD_EXAMPLES)
    add_subdirectory(examples/)
endif()


# --- TESTS ---

option(BYTE_TALK_BUILD_TESTS "Build byte_talk tests" ${PROJECT_IS_TOP_LEVEL})

if(BYTE_TALK_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests/)
endif()


# --- INSTALL ---

# include(GNUInstallDirs)

# install(
#     TARGETS byte_talk
#     EXPORT byte_talkTargets
#     LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#     ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
#     RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
# )
# install(
#     DIRECTORY include/
#     DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
# )


# # --- FIND_PACKAGE ---

# include(CMakePackageConfigHelpers)

# configure_package_config_file(
#     ${CMAKE_CURRENT_SOURCE_DIR}/cmake/byte_talkConfig.cmake.in
#     ${CMAKE_CURRENT_BINARY_DIR}/byte_talkConfig.cmake
#     INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/byte_talk
# )
# install(
#     EXPORT byte_talkTargets
#     FILE byte_talkTargets.cmake
#     NAMESPACE byte_talk::
#     DESTINATION ${CMAEK_INSTALL_LIBDIR}/cmake/byte_talk
# )
# install(
#     FILES ${CMAKE_CURRENT_BINARY_DIR}/byte_talkConfig.cmake
#     DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/byte_talk
# )