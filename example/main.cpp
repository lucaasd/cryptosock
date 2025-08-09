#include "../src/server/server.h"
#include <fmt/base.h>
#include <unistd.h>

int main()
{
    fmt::println("PID: {}", getpid());
    server::Server server(8080);
    server.listen();
}
