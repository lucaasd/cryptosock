#include "server.h"
#include <cstdio>
#include <fmt/base.h>
#include <chrono>
#include <thread>


const int BUFFER_SIZE = 1024;

#if defined(__linux__)

#include <sys/socket.h>
#include <netinet/in.h>

namespace server {
    void Server::listen() {
        // Create a socket

        isRunning = true;

        int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

        // Add an address

        sockaddr_in serverAddress;

        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(port);
        serverAddress.sin_addr.s_addr = INADDR_ANY;

        if (!bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)))
        {
            perror("Something went wrong when binding your socket:");
        }

        // Global listen function
        if (!::listen(serverSocket, 16)) {
            perror("Failed to listen");
        }

        while (isRunning) {
            int clientSocket = accept(serverSocket, nullptr, nullptr);

            char buffer[1024] = { 0 };
            if (!recv(serverSocket, buffer, BUFFER_SIZE, 0)) {
                perror("Error reading message: ");
            }

            fmt::println("Message from client: {}", buffer);
        }
    }

    Server::~Server() {
        fmt::println("Server has been deleted");
    }
}

#elif defined(_WIN64)

#else

#endif
