#include <netinet/in.h>
#ifndef INTERFACESERVICE_H
#define INTERFACESERVICE_H

class InterfaceService {
    public:
        /**
         * @brief Handles the exit message from a client.
         *
         * This function is called when a client sends an exit message. It converts the client's address to a string,
         * locks the clients mutex to ensure thread safety, and checks if the client is in the clients map. If the client
         * is found, it notifies all threads waiting on the client's condition variable and logs the disconnection message.
         * If the client is not found, it logs that the client was not found.
         *
         * @param serverSocket The server socket descriptor.
         * @param clientAddr The address of the client.
         * @param clientAddrLen The length of the client address.
         */
        static void handleExitMessage(int serverSocket, sockaddr_in clientAddr, socklen_t clientAddrLen);
};

#endif
