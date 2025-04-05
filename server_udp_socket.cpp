#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>

#define PORT 4000

///////////////////////////////////

class UdpServer {
    public:

    int sockfd;
    struct sockaddr_in serv_addr, cli_addr;
    char buf[256];
    socklen_t clilen;

        UdpServer(){};

        ~UdpServer() {
            closeServer();
        }

        void initializeServer(int port){
            int create_socket = (sockfd = socket(AF_INET, SOCK_DGRAM, 0));
            if (create_socket == -1) 
                printf("ERROR opening socket");

            serv_addr.sin_family = AF_INET;
            serv_addr.sin_port = htons(port);
            serv_addr.sin_addr.s_addr = INADDR_ANY;
            bzero(&(serv_addr.sin_zero), 8);    
            
            int bind_socket = bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(struct sockaddr));
            if (bind_socket < 0) 
                printf("ERROR on binding");
            
            clilen = sizeof(struct sockaddr_in);
        }

        void startServerLoop(){
            while (true) {
                int n;
                /* receive from socket */
                n = recvfrom(sockfd, buf, 256, 0, (struct sockaddr *) &cli_addr, &clilen);
                if (n < 0) 
                    printf("ERROR on recvfrom");
                printf("Received a datagram: %s\n", buf);
                
                /* send to socket */
                n = sendto(sockfd, "Got your message\n", 17, 0,(struct sockaddr *) &cli_addr, sizeof(struct sockaddr));
                if (n  < 0) 
                    printf("ERROR on sendto");
            }
        }

        void closeServer(){
         close(sockfd);
        }
};

///////////////////////////////////


int main(int argc, char *argv[])
{
    UdpServer udpServer;
    udpServer.initializeServer(PORT);
    udpServer.startServerLoop();
    udpServer.closeServer();
    return 0;
}