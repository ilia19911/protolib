cmake_minimum_required(VERSION 3.28)

message(STATUS "SOFTWARE_VERSION='${SOFTWARE_VERSION}'")
if (NOT DEFINED SOFTWARE_VERSION OR SOFTWARE_VERSION STREQUAL "")
    # fallback, если не пришло из Conan
    set(SOFTWARE_VERSION "0.0.1")
endif()

project(protolib VERSION ${SOFTWARE_VERSION} LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(./include )
include_directories(./)

include(CTest)  # создаёт опцию BUILD_TESTING и если ON — вызовет enable_testing()

if(BUILD_TESTING)
    include(FetchContent)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    )
    FetchContent_MakeAvailable(googletest)
endif()

# Опция покрытия (включай в CI только там, где нужно)
option(SOFTWARE_COVERAGE "Enable coverage flags" OFF)
if(SOFTWARE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(--coverage -O0 -g)
    add_link_options(--coverage)
endif()

add_subdirectory(include/prototypes)
add_subdirectory(libraries)
add_subdirectory(protocols)

include(./install.cmake)





