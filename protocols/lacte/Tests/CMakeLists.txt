add_executable(lacteProtocolTest LacteProtocolTest.cpp)

target_link_libraries(lacteProtocolTest PRIVATE GTest::gtest_main protolib::lacte)
include(GoogleTest)
gtest_discover_tests(lacteProtocolTest)


