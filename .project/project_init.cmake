
set(_expected_vars
        SOFTWARE_VERSION # передается из Conan или из cicd
        SOFTWARE_COVERAGE # опция покрытия, включай в CI только там, где нужно
        BUILD_FOR_TARGET # если нужно собирать для другой платформы, например, для ARM на x86
        BUILD_PACKAGE # если нужно собирать пакет для Conan
)

foreach(var IN LISTS _expected_vars)
    if(DEFINED ${var})
        message(STATUS ">> ${var} = ${${var}}")
    else()
        message(STATUS ">> ${var} is NOT set")
    endif()
endforeach()

if( NOT BUILD_PACKAGE)

    if (APPLE)
        set(PROFILE_BUILD ${CMAKE_CURRENT_LIST_DIR}/Macos_build_conan_profile)
    elseif (UNIX)
        set(PROFILE_BUILD ${CMAKE_CURRENT_LIST_DIR}/Linux_build_conan_profile)
    else ()
        set(PROFILE_BUILD ${CMAKE_CURRENT_LIST_DIR}/Windows_build_conan_profile)
    endif ()

    set(PROFILE_TARGET ${PROFILE_BUILD})

    if(BUILD_FOR_TARGET)
        set(PROFILE_TARGET  ${CMAKE_CURRENT_LIST_DIR}/conan_profile_target)
    endif()

    set(CONAN_BUILD_PATH ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}_conan)

    execute_process(COMMAND ${CMAKE_COMMAND} -E rm -rf "${CONAN_BUILD_PATH}")
    find_program(CONAN_EXECUTABLE
            NAMES conan conan.exe
            HINTS
            $ENV{HOME}/.local/bin
            /usr/local/bin
            /usr/bin
    )
    if (NOT CONAN_EXECUTABLE)
        message(FATAL_ERROR "Conan not found. Add it to PATH or set CONAN_EXECUTABLE.")
    endif()

    execute_process(
            COMMAND ${CONAN_EXECUTABLE} install ${CMAKE_CURRENT_LIST_DIR}
            -pr:b=${PROFILE_BUILD}
            -pr:h=${PROFILE_TARGET}
            --output-folder=${CONAN_BUILD_PATH}
            -s:h build_type=${CMAKE_BUILD_TYPE}
            -r insitech  --build=missing
            WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
            RESULT_VARIABLE CONAN_INSTALL_RESULT
            OUTPUT_VARIABLE CONAN_INSTALL_OUTPUT
            ERROR_VARIABLE  CONAN_INSTALL_ERROR
    )
    if (CONAN_INSTALL_RESULT)
        message(STATUS "Conan output:\n${CONAN_INSTALL_OUTPUT}\n${CONAN_INSTALL_ERROR}")
        message(FATAL_ERROR "Conan install failed with code: ${CONAN_INSTALL_RESULT}")
    endif()

    #option(GCC_VERBOSE "Enable verbose GCC output" ON)
    include(${CONAN_BUILD_PATH}/conan_toolchain.cmake)
endif ()


# Если из CI не прилетело — по умолчанию 0.0.0
if (NOT DEFINED PROJECT_SEMVER)
    set(PROJECT_SEMVER "0.0.0")
endif()

# "Красивая" строка версии для встраивания в бинарь/ресурсы
if (NOT DEFINED SOFTWARE_VERSION)
    set(SOFTWARE_VERSION "${PROJECT_SEMVER}")
endif()


message(STATUS "PROJECT_SEMVER='${PROJECT_SEMVER}'")
message(STATUS "SOFTWARE_VERSION='${SOFTWARE_VERSION}'")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_definitions(SOFTWARE_VERSION="${SOFTWARE_VERSION}")

# Опция покрытия (включай в CI только там, где нужно)
option(SOFTWARE_COVERAGE "Enable coverage flags" OFF)
if(SOFTWARE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(--coverage -O0 -g)
    add_link_options(--coverage)
endif()
