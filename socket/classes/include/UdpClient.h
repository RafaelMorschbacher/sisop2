#ifndef UDPCLIENT_H
#define UDPCLIENT_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define PORT 4000

class UdpClient {
public:
    UdpClient();
    ~UdpClient();

    void initialize(const char *hostname);
    void sendMessage();
    void receiveMessage();

private:
    int sockfd;
    unsigned int length;
    struct sockaddr_in serv_addr, from;
    struct hostent *server;
    char buffer[256];
};

#endif // UDPCLIENT_H