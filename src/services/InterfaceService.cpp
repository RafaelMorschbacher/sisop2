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
#include "InterfaceService.h"
#include "Utils.h"
#include "Global.h"

using namespace std;
void InterfaceService::handleExitMessage(int serverSocket, sockaddr_in clientAddr, socklen_t clientAddrLen)
{
    string clientKey = Utils::addressToString(clientAddr);

    lock_guard<mutex> lock(clientsMutex);
    auto it = clients.find(clientKey);
    if (it != clients.end())
    {
        cout << "Client " << clientKey << " se desconectando..." << endl;
        clients.erase(clientKey);
        it->second.condition_variable.notify_all();
    }
    else
    {
        cout << "Client " << clientKey << " não encontrado" << endl;
    }

}