cmake_minimum_required(VERSION 3.15)

project(fluxins VERSION 1.0.0 LANGUAGES CXX)

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    option(BUILD_TESTS "Build tests" ON)
    option(BUILD_EXAMPLES "Build examples" ON)
    option(BUILD_DOCUMENTATION "Build documentations" ON)
    option(BUILD_PACKAGE "Build package" ON)
else()
    option(BUILD_TESTS "Build tests" OFF)
    option(BUILD_EXAMPLES "Build examples" OFF)
    option(BUILD_DOCUMENTATION "Build documentations" OFF)
    option(BUILD_PACKAGE "Build package" OFF)
endif()

add_subdirectory(source)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()

if(BUILD_EXAMPLES)
    add_subdirectory(example)
endif()

if(BUILD_DOCUMENTATION)
    add_subdirectory(documentation)
endif()

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
        "${CMAKE_BINARY_DIR}/cmake_uninstall.cmake"
        IMMEDIATE @ONLY
    )
    add_custom_target(uninstall
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_BINARY_DIR}/cmake_uninstall.cmake
    )
endif()
