#include "server.h"
#include <asm-generic/socket.h>
#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fmt/base.h>
#include <fmt/format.h>


const int BUFFER_SIZE = 1024;
const std::string errorStrings[] = {"ERR_SUCCESS", "ERR_ZERO_SIZE", "ERR_EMPTY", "ERR_EMPTY_MSG", "ERR_SIZE_MISMATCH", "ERR_EVENT_NULLPTR", "ERR_MESSAGE_NULLPTR"};

#if defined(__linux__)

#include <sys/socket.h>
#include <netinet/in.h>

namespace server {
    int parse(unsigned char *payload, std::string* event, std::string* message, uint16_t totalSize) {
        if (totalSize == 0) {
            return ERR_ZERO_SIZE;
        }

        uint8_t eventSize = payload[0];

        if (eventSize == 0) {
            return ERR_EMPTY;
        }

        const std::string payloadStr(payload, payload + totalSize);

        if (event == nullptr)
        {
            return ERR_EVENT_NULLPTR;
        }

        (*event) = std::string(payload + 1, payload + eventSize + 1);

        uint8_t messageSize = payload[eventSize + 1];

        if (messageSize == 0)
        {
            return ERR_EMPTY_MSG;
        }

        if (messageSize + eventSize + 2 != totalSize)
        {
            return ERR_SIZE_MISMATCH;
        }


        if (message == nullptr) {
            return ERR_MESSAGE_NULLPTR;
        }

        (*message) = std::string(payload + eventSize + 2, payload + eventSize + 2 + messageSize);

        return ERR_SUCCESS;
    }

    std::string sv_strerror(int err) {
        if (err <= std::size(errorStrings) - 1)
        {
            return errorStrings[err];
        }
        return "UNKNOWN";
    }

    // Server class

    void Server::listen() {
        // Create a socket

        isRunning = true;

        _serverSocket = socket(AF_INET, SOCK_STREAM, 0);

        int reuse = 1;
        setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int));

        // Add an address

        sockaddr_in serverAddress;

        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(port);
        serverAddress.sin_addr.s_addr = INADDR_ANY;

        // Bind

        if (bind(_serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) != 0)
        {
            fmt::println("ERRNO: {}/{}", errno, strerror(errno));
            perror("Something went wrong when binding your socket:");
        }

        if (::listen(_serverSocket, 16) != 0 ) { // '::' to avoid conflict
            fmt::println("ERRNO: {}/{}", errno, strerror(errno));
            perror("Failed to listen");
        }

        while (isRunning) {
            int clientSocket = accept(_serverSocket, nullptr, nullptr);

            if (clientSocket > 0)
            {
                char buffer[1024] = { 0 };
                char* bufferPtr = buffer;
                int bytesRead = recv(clientSocket, buffer, BUFFER_SIZE, 0);

                fmt::println("Bytes: {}", bytesRead);
                fmt::println("Errno {}/{}", errno, strerror(errno));

                buffer[bytesRead] = '\0';

                uint8_t eventSize = buffer[0];

                if (eventSize == 0)
                {
                    close(clientSocket);
                    continue;
                }

                std::string message;
                std::string event;
                int size = bytesRead;

                int rc = parse(reinterpret_cast<unsigned char*>(bufferPtr), &event, &message, size);

                if (rc == ERR_SUCCESS)
                {
                    fmt::println("[{}] {}", event, message);

                    if (onMessage) {
                        onMessage(&event, &message, clientSocket, false);
                    }
                } else {
                    fmt::println("ERROR -> {}", sv_strerror(rc));
                }
            }
        }

        close(_serverSocket);
    }

    int Server::serverSocket()
    {
        return _serverSocket;
    }

    Server::~Server() {
        fmt::println("Server has been deleted");
    }
}

#elif defined(_WIN64)

#else

#endif
