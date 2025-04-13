#include "UdpServer.h"

int main() {
    UdpServer server;
    server.initializeServer(4000); // Use port 4000
    server.startServerLoop();

    return 0;
}