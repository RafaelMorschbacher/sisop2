#include "UdpClient.h"
#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

UdpClient::UdpClient(std::string destinationServerIP, int destinationServerPort) 
    : sockfd(-1), length(0), server(nullptr), destinationServerIP(destinationServerIP), destinationServerPort(destinationServerPort) {
    bzero(buffer, 256);
}

UdpClient::~UdpClient() {
    if (sockfd != -1) {
        close(sockfd);
    }
}

void UdpClient::initialize(const char *hostname) {
    server = gethostbyname(hostname);
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("ERROR opening socket");
        exit(1);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr = *((struct in_addr *)server->h_addr);
    bzero(&(serv_addr.sin_zero), 8);
}

void UdpClient::sendMessage() {
    printf("Enter the message: ");
    bzero(buffer, 256);
    fgets(buffer, 256, stdin);

    int n = sendto(sockfd, buffer, strlen(buffer), 0, (const struct sockaddr *)&serv_addr, sizeof(struct sockaddr_in));
    if (n < 0) {
        perror("ERROR sendto");
        exit(1);
    }
}

void UdpClient::receiveMessage() {
    length = sizeof(struct sockaddr_in);
    int n = recvfrom(sockfd, buffer, 256, 0, (struct sockaddr *)&from, &length);
    if (n < 0) {
        perror("ERROR recvfrom");
        exit(1);
    }

    printf("Got an ack: %s\n", buffer);
}