include(GNUInstallDirs)

add_library(protolib_lacte STATIC
#        LacteProtocol.cpp
        ymodem.cpp
)
add_library(protolib::lacte ALIAS protolib_lacte)
set_target_properties(protolib_lacte PROPERTIES EXPORT_NAME lacte)

# Публичные зависимости
target_link_libraries(protolib_lacte PUBLIC
        protolib::fields
        protolib::containers
        protolib::crc16_modbus
)

# Пути к заголовкам (согласованы с установкой FILE_SET ниже)
target_include_directories(protolib_lacte PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
        $<INSTALL_INTERFACE:include>
)

install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/protocols/lacte
        FILES_MATCHING
        PATTERN "*.hpp"
        PATTERN "*.h"
        PATTERN "Tests" EXCLUDE)


if(BUILD_TESTING)
    add_subdirectory(Tests)
endif()
