cmake_minimum_required(VERSION 3.28)

include(./.project/project_init.cmake)

project(protolib VERSION ${PROJECT_SEMVER} LANGUAGES C CXX)

include(CTest)  # создаёт опцию BUILD_TESTING и если ON — вызовет enable_testing()

include_directories(./include )
include_directories(./)

find_package(GTest CONFIG REQUIRED)

add_subdirectory(include/prototypes)
add_subdirectory(libraries)
add_subdirectory(protocols)

include(./install.cmake)
