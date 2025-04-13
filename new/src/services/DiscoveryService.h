#include <netinet/in.h>
#ifndef DISCOVERYSERVICE_H
#define DISCOVERYSERVICE_H

class DiscoveryService {
    public:
        static void handleDiscoveryMessage(int serverSocket, sockaddr_in clientAddr, socklen_t clientAddrLen);
};

#endif
