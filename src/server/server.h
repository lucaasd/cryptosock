#ifndef SERVER
#define SERVER

#include <string>
#include <atomic>

extern const int BUFFER_SIZE;

namespace server {

typedef void (*ServerCallback)(const std::string& event, bool internal, const std::string& data, const std::string& to);

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
    };

}
#endif
