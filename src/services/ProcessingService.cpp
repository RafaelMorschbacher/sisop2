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
using namespace std;

void ProcessingService::processMessage(int serverSocket, const string &clientKey)
{
    ClientInfo &clientInfo = clients[clientKey];

    while (true)
    {
        // Espera por uma mensagem na fila para processar
        // Se a fila estiver vazia, espera até que uma nova mensagem chegue
        // Se a fila não estiver vazia, processa a mensagem
        {
            unique_lock<mutex> lock(clientInfo.queueMutex);
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
                lock_guard<mutex> lock(clientInfo.queueMutex);
                msg = clientInfo.messageQueue.front();
                clientInfo.messageQueue.pop();
            }
            string response;
            istringstream iss(msg.content);
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
                    // Atualiza a soma global e o número de requisições com um mutex
                    {
                        lock_guard<mutex> lock(globalVarMutex);
                        globalSum += clientNumber;
                        clientInfo.lastSum = globalSum;
                        clientInfo.lastReq = requestId;
                        serverInfo.num_reqs++;
                        serverInfo.total_sum = globalSum;
                    }


                    response = /* Utils::getCurrentTime() + " client " + Utils::addressToString(msg.clientAddr) + */" id_req " + std::to_string(requestId) + " value " + std::to_string(clientNumber) + " num_reqs " + std::to_string(serverInfo.num_reqs) + " total_sum " + std::to_string(globalSum);
                }
                else
                {
                    if(requestId <= clientInfo.lastReq)
                    {
                        std::cout << Utils::getCurrentTime() + " client " +  Utils::addressToString(msg.clientAddr) + " DUP!! " "id_req " + std::to_string(requestId) + " value " + std::to_string(clientNumber) + " num_reqs " + std::to_string(serverInfo.num_reqs) + " total_sum " + std::to_string(globalSum) << std::endl;
                    }
                    response = "Número de request incorreto";
                }
            }
            ssize_t sentBytes = sendto(serverSocket, response.c_str(), response.size(), 0,
                                    (struct sockaddr *)&msg.clientAddr, msg.clientAddrLen);
            if (sentBytes < 0)
            {
                cerr << "Erro ao enviar response ao client " << clientKey << "." << endl;
            }
            else
            {
                cout << Utils::getCurrentTime() + " client " + Utils::addressToString(msg.clientAddr) + " " + response << endl;
            }
        }
    }
    {
        lock_guard<mutex> clientsLock(clientsMutex);
        clients.erase(clientKey);
    }
}
