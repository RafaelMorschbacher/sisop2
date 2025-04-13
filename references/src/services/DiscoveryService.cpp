#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <vector>
#include <queue>
#include <condition_variable>
#include <mutex>
#include <sstream>
#include <unordered_map>
#include <ctime>
#include "DiscoveryService.h"
#include "Utils.h"
#include "Global.h"
#include "ProcessingService.h"


void DiscoveryService::handleDiscoveryMessage(int serverSocket, sockaddr_in clientAddr, socklen_t clientAddrLen)
{
    FILE *file = fopen("src/log_log.log", "w");
    std::string clientKey = Utils::addressToString(clientAddr);

    std::lock_guard<std::mutex> lock(clientsMutex);
    if (clients.find(clientKey) != clients.end())
    {
        std::cout << "Client " << clientKey << " is already connected." << std::endl;
        return;
    }

    // If the client is not already connected, create a new entry for it and a new thread to process its messages
    ClientInfo &clientInfo = clients[clientKey];
    clientInfo.lastReq = 0;
    clientInfo.lastSum = 0;
    clientInfo.workerThread = std::thread(ProcessingService::processMessage, serverSocket, clientKey);

    // Send the handshake to the client
    ssize_t sentBytes = sendto(serverSocket, ACKNOWLEDGEDMESSAGE, 12, 0, (struct sockaddr *)&clientAddr, clientAddrLen);
    if (sentBytes < 0)
    {
        std::cerr << "Error sending acknowledge message to client " << clientKey << "." << std::endl;
        fprintf(file, "Error sending acknowledge message to client %s.\n", clientKey.c_str());
    }
    else
    {
        std::cout << "Handshake sent to client " << clientKey << "." << std::endl;
        fprintf(file, "Handshake sent to client %s.\n", clientKey.c_str());
    }
}
