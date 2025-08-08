#include <chrono>
#include <fmt/base.h>
#include <gtest/gtest.h>
#include <fmt/format.h>
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
