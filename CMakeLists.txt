cmake_minimum_required(VERSION 3.10)

project(MyProtocol)

find_package(GTest CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)

enable_testing()

file(GLOB_RECURSE SOURCES src/*.cpp)
file(GLOB_RECURSE TESTS test/*.cpp)

add_library(CryptoSock ${SOURCES})
target_link_libraries(CryptoSock PRIVATE fmt)

add_executable(CryptoSockExample example/main.cpp)
target_link_libraries(CryptoSockExample CryptoSock fmt)

add_executable(Tests ${TESTS})
target_link_libraries(Tests PRIVATE GTest::gtest GTest::gtest_main fmt CryptoSock)
target_include_directories(Tests PRIVATE ${CMAKE_SOURCE_DIR}/src)
