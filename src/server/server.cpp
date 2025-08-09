#include "server.h"
#include <cerrno>
#include <cstdio>
#include <cstring>
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

        if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) != 0)
        {
            fmt::println("ERRNO: {} - {}", errno, strerror(errno));
            perror("Something went wrong when binding your socket:");
        }

        // Global listen function
        if (::listen(serverSocket, 16) != 0) {
            fmt::println("ERRNO: {}/{}", errno, strerror(errno));
            perror("Failed to listen");
        }

        while (isRunning) {
            int clientSocket = accept(serverSocket, nullptr, nullptr);

            if (clientSocket > 0)
            {
                char buffer[1024] = { 0 };
                int bytesRead = recv(clientSocket, buffer, BUFFER_SIZE, 0);

                fmt::println("Bytes: {}", bytesRead);
                fmt::println("Errno {}/{}", errno, strerror(errno));

                buffer[bytesRead] = '\0';

                fmt::println("Message from client: {}", buffer);
            }
        }

        close(serverSocket);
    }

    Server::~Server() {
        fmt::println("Server has been deleted");
    }
}

#elif defined(_WIN64)

#else

#endif
