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
#include "ProcessingService.h"
#include "InterfaceService.h"
#include "Utils.h"
#include "Message.h"
#include "ServerInfo.h"
#include "ClientInfo.h"
#include "Global.h"

class ServerHandler
{
public:
    ServerHandler(int port) : port(port), serverSocket(-1) {}

    bool initialize()
    {
        // Criação de socket UDP
        serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
        if (serverSocket < 0)
        {
            std::cerr << "Error creating server socket." << std::endl;
            return false;
        }

        // Configuração do endereço do servidor
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_port = htons(port);

        // Faz o bind do socket ao endereço e porta
        if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
        {
            std::cerr << "Error binding socket to address." << std::endl;
            close(serverSocket);
            return false;
        }

        std::cout << "UDP server started on port " << port << "." << std::endl;
        std::cout << "Main thread ID: " << std::this_thread::get_id() << "." << std::endl;
        std::cout << "Current sum: " << std::to_string(globalSum) << std::endl;
        std::cout << "Total requests: " << std::to_string(serverInfo.num_reqs) << std::endl;

        return true;
    }

    void startListening()
    {
        char buffer[BUFFER_SIZE];

        // Loop principal para receber mensagens de clients
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
            std::string message(buffer);

            // Verifica se a mensagem é uma mensagem de broadcast
            if (std::strcmp(message.c_str(), BROADCAST_MESSAGE) == 0)
            {
                DiscoveryService::handleDiscoveryMessage(serverSocket, clientAddr, clientAddrLen);
                continue;
            }

            // Verifica se a mensagem é uma mensagem de desconexão
            if (std::strcmp(message.c_str(), EXIT_MESSAGE) == 0)
            {
                InterfaceService::handleExitMessage(serverSocket, clientAddr, clientAddrLen);
                continue;
            }

            // Se a mensagem não for um broadcast ou desconexão, trata como uma mensagem normal
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
                    std::cout << "Message received from unregistered client: " << clientKey << "." << std::endl;
                }
            }
        }
    }

    ~ServerHandler()
    {
        if (serverSocket >= 0)
        {
            close(serverSocket);
        }
    }

private:
    int port;
    int serverSocket;
    sockaddr_in serverAddr, clientAddr;
};

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
        return 1;
    }

    int port = std::atoi(argv[1]);
    if (port <= 0)
    {
        std::cerr << "Invalid port number" << std::endl;
        return 1;
    }

    ServerHandler serverHandler(port);
    if (!serverHandler.initialize())
    {
        return -1;
    }

    serverHandler.startListening();
    return 0;
}
