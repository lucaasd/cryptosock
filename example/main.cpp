#include "../src/server/server.h"
#include <csignal>
#include <cstdlib>
#include <fmt/base.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

void onMessage(const std::string* event, const std::string* message, int target, bool internal) {

    std::string ping("PING");
    if (*event == ping) {
        std::string pong("PONG");
        send(target, pong.c_str(), pong.size(), 0);
    }
}

int serverSocket;

void segfault_handler(int sig)
{
    close(serverSocket);
    exit(1);
}

int main()
{
    server::Server server = server::Server(8080);
    serverSocket = server.serverSocket();
    fmt::println("PID: {}", getpid());
    signal(SIGSEGV, segfault_handler);
    server.listen();
    server.onMessage = onMessage;
}
