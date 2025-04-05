#include "UdpClient.h"

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