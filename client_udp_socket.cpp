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

////////////////////////////////

class UdpClient {
public:
    int sockfd;
    unsigned int length;
    struct sockaddr_in serv_addr, from;
    struct hostent *server;
    char buffer[256];

    UdpClient() : sockfd(-1), n(0), length(0), server(nullptr) {
        bzero(buffer, 256);
    }

    ~UdpClient() {
        if (sockfd != -1) {
            close(sockfd);
        }
    }

    void initialize(const char *hostname) {
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

    void sendMessage() {
        printf("Enter the message: ");
        bzero(buffer, 256);
        fgets(buffer, 256, stdin);

        int n = sendto(sockfd, buffer, strlen(buffer), 0, (const struct sockaddr *)&serv_addr, sizeof(struct sockaddr_in));
        if (n < 0) {
            perror("ERROR sendto");
            exit(1);
        }
    }

    void receiveMessage() {
        length = sizeof(struct sockaddr_in);
        int n = recvfrom(sockfd, buffer, 256, 0, (struct sockaddr *)&from, &length);
        if (n < 0) {
            perror("ERROR recvfrom");
            exit(1);
        }

        printf("Got an ack: %s\n", buffer);
    }
};

////////////////////////////////

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "usage %s hostname\n", argv[0]);
        exit(0);
    }

    const char *host_name = argv[1];

    UdpClient client;
    client.initialize(host_name);
    client.sendMessage();
    client.receiveMessage();

    return 0;
}