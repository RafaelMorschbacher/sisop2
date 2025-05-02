#include <iostream>
#include <cstring>
#include <regex>
#include <arpa/inet.h>
#include <unistd.h>
#include "Global.h"
#include "Utils.h"
#include "ClientHandler.h"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Uso: " << argv[0] << " <broadcast port>" << std::endl;
        return 1;
    }

    int broadcastPort = std::atoi(argv[1]);
    if (broadcastPort <= 0)
    {
        std::cerr << "Numero de porta invalido" << std::endl;
        return 1;
    }

    ClientHandler clientHandler;

    if (!clientHandler.initializeSocket(broadcastPort))
    {
        return 1;
    }

    if (!clientHandler.performHandshake())
    {
        return 1;
    }

    clientHandler.mainLoop();

    return 0;
}
