#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

#include <iostream>
#include <cstring>
#include <regex>
#include <arpa/inet.h>
#include <unistd.h>
#include "Global.h"
#include "Utils.h"

class ClientHandler
{
public:
    ClientHandler();
    ~ClientHandler();

    bool initializeSocket(int broadcastPort);
    bool findServer();
    void mainLoop();

private:
    int clientSocket;
    sockaddr_in broadcastAddr, serverAddr;
    char buffer[BUFFER_SIZE];
    int requestId;
};

#endif // CLIENT_HANDLER_H
