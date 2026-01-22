cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 17)

project(NetSim)

add_compile_options(-Wall -Wextra -Werror -Wpedantic -pedantic-errors)

include_directories(include)

set(SOURCE_FILES
        src/package.cpp
        src/storage_types.cpp
        src/nodes.cpp
        src/helpers.cpp
        src/factory.cpp)

add_executable(NetSim_debug
        ${SOURCE_FILES}
        main.cpp)

set(SOURCE_FILES_TESTS
        test/test_package.cpp
        test/test_storage_types.cpp
        test/test_nodes.cpp
        test/test_Factory.cpp
        test/test_factory_io.cpp)

add_executable(NetSim_test ${SOURCE_FILES} ${SOURCE_FILES_TESTS}
        test/main_gtest.cpp)

target_include_directories(NetSim_test PUBLIC
        googletest-master/googlemock/include
        googletest-master/googletest/include
        mocks)

add_subdirectory(googletest-master)

add_definitions(-DWITH_PROBABILITY_GENERATOR)

target_link_libraries(NetSim_test gmock)