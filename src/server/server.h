#ifndef SERVER
#define SERVER

#include <cstdint>
#include <fmt/format.h>
#include <string>
#include <atomic>

extern const int BUFFER_SIZE;
extern const std::string errStrings[];


namespace server {

enum ParseError {
    ERR_SUCCESS = 0,
    ERR_ZERO_SIZE = 1,
    ERR_EMPTY = 2,
    ERR_EMPTY_MSG = 3,
    ERR_SIZE_MISMATCH = 4,
    ERR_EVENT_NULLPTR = 5,
    ERR_MESSAGE_NULLPTR = 6,
};

typedef void (*ServerCallback)(const std::string* event, const std::string* data, int target, bool internal);

class Server {
  public:
    Server(unsigned int port) : port(port) {}
    ~Server();
    unsigned int port;
    void listen();
    std::atomic<bool> isRunning;
    ServerCallback onMessage;
    ServerCallback onDestroy;
    ServerCallback onStart;
    int serverSocket();
  private:
    int _serverSocket;
};

int parse(unsigned char* payload, std::string* event, std::string* message, uint16_t size);
}

std::string sv_strerror(int err);
#endif
