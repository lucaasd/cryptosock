#include "server.h"
#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fmt/base.h>


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

        // Bind

        if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) != 0)
        {
            fmt::println("ERRNO: {}/{}", errno, strerror(errno));
            perror("Something went wrong when binding your socket:");
        }

        if (::listen(serverSocket, 16) != 0 ) { // '::' to avoid conflict
            fmt::println("ERRNO: {}/{}", errno, strerror(errno));
            perror("Failed to listen");
        }

        while (isRunning) {
            int clientSocket = accept(serverSocket, nullptr, nullptr);

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

                parse(reinterpret_cast<unsigned char*>(bufferPtr), &event, &message, size);

                fmt::println("Event: {} Message: {}", event, message);
            }
        }

        close(serverSocket);
    }

    Server::~Server() {
        fmt::println("Server has been deleted");
    }

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

        (*event) = std::string(payload + 1, payload + 1 + eventSize);

        uint8_t messageSize = payload[eventSize + 1];

        if (messageSize == 0)
        {
            return ERR_EMPTY_MSG;
        }


        if (message == nullptr) {
            return ERR_MESSAGE_NULLPTR;
        }

        (*message) = std::string(payload + eventSize + 2, payload + eventSize + 2 + messageSize);

        return ERR_SUCCESS;
    }
}

#elif defined(_WIN64)

#else

#endif
