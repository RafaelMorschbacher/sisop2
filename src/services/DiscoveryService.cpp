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
using namespace std;

void DiscoveryService::handleDiscoveryMessage(int serverSocket, sockaddr_in clientAddr, socklen_t clientAddrLen)
{
    string clientKey = Utils::addressToString(clientAddr);

    lock_guard<mutex> lock(clientsMutex);
    if (clients.find(clientKey) != clients.end())
    {
        cout << "Client " << clientKey << " já está conectado." << endl;
        return;
    }

    // Se o cliente não estiver conectado, cria uma nova entrada para ele e uma nova thread para processar suas mensagens
    ClientInfo &clientInfo = clients[clientKey];
    clientInfo.lastReq = 0;
    clientInfo.lastSum = 0;
    clientInfo.workerThread = thread(ProcessingService::processMessage, serverSocket, clientKey);

    // Envia mensagem de acknowledgment para o client
    ssize_t sentBytes = sendto(serverSocket, ACKNOWLEDGEDMESSAGE, 12, 0, (struct sockaddr *)&clientAddr, clientAddrLen);
    if (sentBytes < 0)
    {
        cerr << "Erro ao enviar mensagem de acknowledge para o client  " << clientKey << "." << endl;
    }

}
