add_executable(ContainerTests ContainerTests.cpp
        RxContainerTest.cpp
        TxContainerTest.cpp
        Prototypes.cpp
        ../../field/Tests/FieldPrototypes.cpp
        PingPongTest.cpp)

target_link_libraries(ContainerTests PRIVATE protolib::fields protolib::containers GTest::gtest_main)
target_include_directories(ContainerTests PRIVATE . ../../field/Tests)
include(GoogleTest)
gtest_discover_tests(ContainerTests)


