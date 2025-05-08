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
        std::cerr << "Erro ao criar socket no servidor." << std::endl;
        return false;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        std::cerr << "Erro ao fazer o bind do socket ao endereço" << std::endl;
        close(serverSocket);
        return false;
    }

    std::cout << Utils::getCurrentTime() + " num_reqs " << std::to_string(serverInfo.num_reqs) << + " total_sum " << std::to_string(globalSum) << std::endl;

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
            std::cerr << "Erro ao receber dados" << std::endl;
            continue;
        }

        buffer[recvBytes] = '\0';
        std::string message(buffer);

        if (std::strcmp(message.c_str(), BROADCAST_MESSAGE) == 0)
        {
            DiscoveryService::handleDiscoveryMessage(serverSocket, clientAddr, clientAddrLen);
            continue;
        }

        if (std::strcmp(message.c_str(), EXIT_MESSAGE) == 0)
        {
            InterfaceService::handleExitMessage(serverSocket, clientAddr, clientAddrLen);
            continue;
        }

        std::string clientKey = Utils::addressToString(clientAddr);
        {
            std::lock_guard<std::mutex> lock(clientsMutex);
            auto it = clients.find(clientKey);
            if (it != clients.end())
            {
                {
                    std::lock_guard<std::mutex> queueLock(it->second.queueMutex);
                    it->second.messageQueue.push(Message{message, clientAddr, clientAddrLen});
                }
                it->second.condition_variable.notify_one();
            }
            else
            {
                std::cout << "Mensagem recebida de um cliente não registrado na lista de clientes: " << clientKey << "." << std::endl;
            }
        }
    }
}
