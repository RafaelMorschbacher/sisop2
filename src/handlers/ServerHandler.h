#ifndef SERVER_HANDLER_H
#define SERVER_HANDLER_H

#include <arpa/inet.h>

class ServerHandler
{
public:
    explicit ServerHandler(int port);
    ~ServerHandler();

    bool initialize();
    void startListening();

private:
    int port;
    int serverSocket;
    sockaddr_in serverAddr, clientAddr;
};

#endif
