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
#include "ServerHandler.h"
#include "Utils.h"
#include "Message.h"
#include "ServerInfo.h"
#include "ClientInfo.h"
#include "Global.h"


int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Uso: " << argv[0] << " <port>" << std::endl;
        return 1;
    }

    int port = std::atoi(argv[1]);
    if (port <= 0)
    {
        std::cerr << "Numero de porta invalido" << std::endl;
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
