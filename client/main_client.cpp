#include "UdpClient.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "usage %s hostname\n", argv[0]);
        exit(0);
    }

    const char *host_name = argv[1];

    UdpClient client("1234.12345", 1234); // IP and port not being used yet
    client.initialize(host_name);
    client.sendMessage();
    client.receiveMessage();

    return 0;
}