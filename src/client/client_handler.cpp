#include <iostream>
#include <cstring>
#include <regex>
#include <arpa/inet.h>
#include <unistd.h>
#include "Global.h"
#include "Utils.h"
#include "ClientHandler.h"
using namespace std;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cerr << "Uso: " << argv[0] << " <broadcast port>" << endl;
        return 1;
    }

    int broadcastPort = atoi(argv[1]);
    if (broadcastPort <= 0)
    {
        cerr << "Numero de porta invalido" << endl;
        return 1;
    }

    ClientHandler clientHandler;

    if (!clientHandler.initializeSocket(broadcastPort))
    {
        return 1;
    }

    if (!clientHandler.findServer())
    {
        return 1;
    }

    clientHandler.mainLoop();

    return 0;
}
