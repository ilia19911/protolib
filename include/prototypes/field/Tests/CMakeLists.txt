add_executable(fieldTests FieldTests.cpp
        DataFieldTests.cpp
        FieldPrototypes.hpp
        FieldPrototypes.cpp)
target_link_libraries(fieldTests PRIVATE protolib::fields GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(fieldTests)


