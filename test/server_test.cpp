#include <chrono>
#include <fmt/base.h>
#include <gtest/gtest.h>
#include <fmt/format.h>
#include <stdexcept>
#include <string>
#include <thread>
#include "../src/server/server.h"

TEST(ServerTests, ListenTest) {
    server::Server server(3000);

    std::thread([&server] {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        server.isRunning = false;
    }).detach();

    std::this_thread::sleep_for(std::chrono::seconds(2));

    EXPECT_FALSE(server.isRunning);
}

TEST(ServerUtilsTest, PayloadParseWithZeroSizeTest) {
    unsigned char payload[] = "";
    unsigned char* ptr = payload;

    EXPECT_EQ(server::parse(ptr, nullptr, nullptr, 0), server::ERR_ZERO_SIZE);
}

TEST(ServerUtilsTest, PayloadParseWithEmptyStringTest) {
    unsigned char payload[] = "\0aaaaa";
    unsigned char* ptr = payload;

    EXPECT_EQ(server::parse(ptr, nullptr, nullptr, 20), server::ERR_EMPTY);
}

TEST(ServerUtilTest, PayloadParseWithoutMessageTest) {
    unsigned char payload[] = {0x04, 'a', 'b', 'c', 'd'};
    unsigned char* ptr = payload;

    std::string event;

    int rc = server::parse(ptr, &event, nullptr, std::size(payload));

    fmt::println("Event: {}", event);
    fmt::println("RC: {}", rc);

    EXPECT_EQ(rc, server::ERR_EMPTY_MSG);
    EXPECT_EQ(event, std::string("abcd"));
}

TEST(ServerUtilTest, PayloadParseWithMessageTest)
{
    unsigned char payload[] = {0x04, 'a', 'b', 'c', 'd', 0x03, 'e', 'f', 'g'};
    unsigned char* ptr = payload;

    std::string event;
    std::string message;

    int rc = server::parse(ptr, &event, &message, std::size(payload));

    fmt::println("Event: {}", event);
    fmt::println("RC: {}", rc);

    EXPECT_EQ(rc, server::ERR_SUCCESS);
    EXPECT_EQ(event, std::string("abcd"));
    EXPECT_EQ(message, std::string("efg"));
}

TEST(ServerUtilTest, PayloadParseWithWrongMessageSize) {
    unsigned char payload[] = {0x04, 'a', 'b', 'c', 'd', 0x05, 'e', 'f', 'g'};
    unsigned char* ptr = payload;


    std::string event;
    std::string message;

    int rc = server::parse(ptr, &event, &message, std::size(payload));

    fmt::println("RC: {}", rc);

    EXPECT_EQ(rc, server::ERR_SIZE_MISMATCH);
}
