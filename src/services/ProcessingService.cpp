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
#include "ProcessingService.h"
#include "ClientInfo.h"
#include "Global.h"
#include "Utils.h"
     
void ProcessingService::processMessage(int serverSocket, const std::string &clientKey)
{
    ClientInfo &clientInfo = clients[clientKey];

    while (true)
    {
        // Espera por uma mensagem na fila para processar
        // Se a fila estiver vazia, espera até que uma nova mensagem chegue
        // Se a fila não estiver vazia, processa a mensagem
        {
            std::unique_lock<std::mutex> lock(clientInfo.queueMutex);
            clientInfo.condition_variable.wait(lock, [&clientInfo]()
                                            { return !clientInfo.messageQueue.empty(); });

            if (clientInfo.messageQueue.empty())
            {
                break;
            }
        }

        if (!clientInfo.messageQueue.empty())
        {
            Message msg;

            // Pega a mensagem do top da fila
            {
                std::lock_guard<std::mutex> lock(clientInfo.queueMutex);
                msg = clientInfo.messageQueue.front();
                clientInfo.messageQueue.pop();
            }

            std::string response;
            std::istringstream iss(msg.content);
            int requestId, clientNumber;
            iss >> requestId >> clientNumber;

            if (iss.fail() || !iss.eof())
            {
                response = "Invalid message format.";
                clientInfo.lastReq = requestId;
            }
            else
            {
                // Verifica se o ID da requisição é o ID esperado
                if (requestId == clientInfo.lastReq + 1)
                {
                    clientInfo.lastReq = requestId;

                    // Atualiza a soma global e o número de requisições com um mutex
                    {
                        std::lock_guard<std::mutex> lock(globalVarMutex);
                        globalSum += clientNumber;
                        clientInfo.lastSum = globalSum;
                        serverInfo.num_reqs++;
                        serverInfo.total_sum = globalSum;
                    }


                    response = "Date " + Utils::getCurrentTime() + " - Number of requests: " + std::to_string(serverInfo.num_reqs) + " - Total accumulated: " + std::to_string(globalSum) + " (Thread ID: " + std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id())) + ")";
                }
                else
                {
                    response = "Incorrect request number.";
                }
            }
            ssize_t sentBytes = sendto(serverSocket, response.c_str(), response.size(), 0,
                                    (struct sockaddr *)&msg.clientAddr, msg.clientAddrLen);
            if (sentBytes < 0)
            {
                std::cerr << "Error sending response to client " << clientKey << "." << std::endl;
            }
            else
            {
                std::cout << "Response sent to client " << clientKey << ": " << response << std::endl;
            }
        }
    }

    {
        std::lock_guard<std::mutex> clientsLock(clientsMutex);
        clients.erase(clientKey);
    }
}
