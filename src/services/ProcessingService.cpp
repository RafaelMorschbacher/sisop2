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
                response = "Formato de mensagem invalido.";
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


                    response = /* Utils::getCurrentTime() + " client " + Utils::addressToString(msg.clientAddr) + */" id_req " + std::to_string(requestId) + " value " + std::to_string(clientNumber) + " num_reqs " + std::to_string(serverInfo.num_reqs) + " total_sum " + std::to_string(globalSum);
                }
                else
                {
                    if(requestId <= clientInfo.lastReq)
                    {
                        response = " DUP!! " "id_req " + std::to_string(requestId) + " value " + std::to_string(clientNumber) + " num_reqs " + std::to_string(serverInfo.num_reqs) + " total_sum " + std::to_string(globalSum);
                    }
                    // response = "Número de request incorreto";
                }
            }
            ssize_t sentBytes = sendto(serverSocket, response.c_str(), response.size(), 0,
                                    (struct sockaddr *)&msg.clientAddr, msg.clientAddrLen);
            if (sentBytes < 0)
            {
                std::cerr << "Erro ao enviar response ao client " << clientKey << "." << std::endl;
            }
            else
            {
                std::cout << Utils::getCurrentTime() + " client " + Utils::addressToString(msg.clientAddr) + " " + response << std::endl;
            }
        }
    }

    {
        std::lock_guard<std::mutex> clientsLock(clientsMutex);
        clients.erase(clientKey);
    }
}
