#include <netinet/in.h>
#include <string>
#ifndef PROCESSINGSERVICE_H
#define PROCESSINGSERVICE_H

class ProcessingService {
    public:
        /**
         * @brief Processes messages from a client in a loop until the message queue is empty.
         *
         * This function continuously waits for messages from a specific client, processes them,
         * and sends appropriate responses back to the client. It updates the global sum and server
         * information based on the client's requests.
         *
         * @param serverSocket The socket descriptor for the server.
         * @param clientKey The unique key identifying the client.
         *
         *
         * The function uses mutexes to ensure thread safety when accessing shared resources
         * such as the client's message queue, global sum, and server information.
         */
        static void processMessage(int serverSocket, const std::string &clientKey);

};

#endif
