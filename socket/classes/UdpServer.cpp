#include "UdpServer.h"

UdpServer::UdpServer() : sockfd(-1), clilen(0) {
    bzero(buf, 256);
}

UdpServer::~UdpServer() {
    closeServer();
}

void UdpServer::initializeServer(int port) {
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        perror("ERROR opening socket");
        return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(serv_addr.sin_zero), 8);

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) < 0) {
        perror("ERROR on binding");
        return;
    }

    clilen = sizeof(struct sockaddr_in);
}

void UdpServer::startServerLoop() {
    while (true) {
        int n = recvfrom(sockfd, buf, 256, 0, (struct sockaddr *)&cli_addr, &clilen);
        if (n < 0) {
            perror("ERROR on recvfrom");
            continue;
        }

        printf("Received message: %s\n", buf);

        // Send acknowledgment back to the client
        n = sendto(sockfd, "ACK", 3, 0, (struct sockaddr *)&cli_addr, clilen);
        if (n < 0) {
            perror("ERROR on sendto");
        }
    }
}

void UdpServer::closeServer() {
    if (sockfd != -1) {
        close(sockfd);
    }
}