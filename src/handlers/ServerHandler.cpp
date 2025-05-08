#include "ServerHandler.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <thread>
#include "DiscoveryService.h"
#include "InterfaceService.h"
#include "Utils.h"
#include "Message.h"
#include "ServerInfo.h"
#include "ClientInfo.h"
#include "Global.h"

using namespace std;

ServerHandler::ServerHandler(int port) : port(port), serverSocket(-1) {}

ServerHandler::~ServerHandler()
{
    if (serverSocket >= 0)
    {
        close(serverSocket);
    }
}

bool ServerHandler::initialize()
{
    serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (serverSocket < 0)
    {
        std::cerr << "Error creating server socket." << std::endl;
        return false;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        std::cerr << "Error binding socket to address." << std::endl;
        close(serverSocket);
        return false;
    }

    cout << Utils::getCurrentTime() + " num_reqs " << to_string(serverInfo.num_reqs) << + " total_sum " << to_string(globalSum) << endl;

    return true;
}

void ServerHandler::startListening()
{
    char buffer[BUFFER_SIZE];
    while (true)
    {
        socklen_t clientAddrLen = sizeof(clientAddr);
        int recvBytes = recvfrom(serverSocket, buffer, BUFFER_SIZE - 1, 0,
                                 (struct sockaddr *)&clientAddr, &clientAddrLen);
        if (recvBytes <= 0)
        {
            std::cerr << "Error receiving data." << std::endl;
            continue;
        }

        buffer[recvBytes] = '\0';
        string message(buffer);

        if (strcmp(message.c_str(), BROADCAST_MESSAGE) == 0)
        {
            DiscoveryService::handleDiscoveryMessage(serverSocket, clientAddr, clientAddrLen);
            continue;
        }

        if (strcmp(message.c_str(), EXIT_MESSAGE) == 0)
        {
            InterfaceService::handleExitMessage(serverSocket, clientAddr, clientAddrLen);
            continue;
        }

        string clientKey = Utils::addressToString(clientAddr);
        {
            lock_guard<mutex> lock(clientsMutex);
            auto it = clients.find(clientKey);
            if (it != clients.end())
            {
                {
                    lock_guard<mutex> queueLock(it->second.queueMutex);
                    it->second.messageQueue.push(Message{message, clientAddr, clientAddrLen});
                }
                it->second.condition_variable.notify_one();
            }
            else
            {
                std::cout << "Message received from unregistered client: " << clientKey << "." << std::endl;
            }
        }
    }
}
