#ifndef UDPSERVER_H
#define UDPSERVER_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

class UdpServer {
public:
    UdpServer();
    ~UdpServer();

    void initializeServer(int port);
    void startServerLoop();
    void closeServer();

private:
    int sockfd;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    char buf[256];
};

#endif // UDPSERVER_H