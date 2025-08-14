namespace Client {
    class Client {
      public:
        Client(int targetPort) : targetPort(targetPort) {}
        int targetPort;
        ~Client();
    };
}
