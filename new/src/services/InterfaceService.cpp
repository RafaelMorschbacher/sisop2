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

void InterfaceService::handleExitMessage(int serverSocket, sockaddr_in clientAddr, socklen_t clientAddrLen)
{
    std::string clientKey = Utils::addressToString(clientAddr);

    std::lock_guard<std::mutex> lock(clientsMutex);
    auto it = clients.find(clientKey);
    if (it != clients.end())
    {
        it->second.condition_variable.notify_all();
        std::cout << "Client " << clientKey << " is disconnecting..." << std::endl;
    }
    else
    {
        std::cout << "Client " << clientKey << " not found." << std::endl;
    }
}