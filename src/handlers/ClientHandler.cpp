#include "ClientHandler.h"
#include "Utils.h"
#include <unistd.h>
using namespace std;

ClientHandler::ClientHandler() : clientSocket(-1), requestId(0) {}

ClientHandler::~ClientHandler()
{
    if (clientSocket >= 0)
    {
        close(clientSocket);
    }
}

bool ClientHandler::initializeSocket(int broadcastPort)
{
    clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (clientSocket < 0)
    {
        cerr << "Erro ao criar socket" << endl;
        return false;
    }

    
struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 10000;

    if (setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) 
    {
        cerr << "Erro ao habilitar timeout" << endl;
        close(clientSocket);
        return 1;
    }

    int broadcastEnable = 1;
    if (setsockopt(clientSocket, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)) > 0)
    {
        cerr << "Erro ao habilitar socket" << endl;
        close(clientSocket);
        return false;
    }

    memset(&broadcastAddr, 0, sizeof(broadcastAddr));
    broadcastAddr.sin_family = AF_INET;
    broadcastAddr.sin_port = htons(broadcastPort);
    broadcastAddr.sin_addr.s_addr = htonl(INADDR_BROADCAST);

    return true;
}

bool ClientHandler::findServer()
{
    int broadcastReturn = sendto(clientSocket, BROADCAST_MESSAGE, strlen(BROADCAST_MESSAGE), 0, (struct sockaddr *)&broadcastAddr, sizeof(broadcastAddr));
    if (broadcastReturn < 0)
    {
        cerr << "Erro ao enviar mensagem de broadcast" << endl;
        return false;
    }

    socklen_t serverAddrLen = sizeof(serverAddr);
    int recvBytes = recvfrom(clientSocket, buffer, BUFFER_SIZE - 1, 0, (struct sockaddr *)&serverAddr, &serverAddrLen);
    if (recvBytes < 0)
    {
        cerr << "Erro ao receber resposta do servidor." << endl;
        return false;
    }

    buffer[recvBytes] = '\0';
    if (strcmp(buffer, ACKNOWLEDGEDMESSAGE) == 0)
    {
        cout << Utils::getCurrentTime() + " server_addr " +  Utils::addressToString(serverAddr) << endl;
        return true;
    }
    else
    {
        cout << "Handshake com o servidor falhou" << endl;
        return false;
    }
}

void ClientHandler::mainLoop()
{
    string message;
    while (true)
    {
        
        getline(cin, message);
        if (cin.eof() || message == EXIT_MESSAGE)
        {
            cout << "Desconectando do servidor..." << endl;
            sendto(clientSocket, EXIT_MESSAGE, 4, 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
            break;
        }

        string fullMessage = to_string(++requestId) + " " + message;

        sendto(clientSocket, fullMessage.c_str(), fullMessage.size(), 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

        socklen_t serverAddrLen = sizeof(serverAddr);
        int recvBytes = recvfrom(clientSocket, buffer, BUFFER_SIZE - 1, 0, (struct sockaddr *)&serverAddr, &serverAddrLen);
        while(recvBytes < 0)
        {

            if (errno == EAGAIN || errno == EWOULDBLOCK) 
            {
                cerr << "Erro ao receber resposta do servidor, iniciando retransmissão depois de 10 milisegundos." << endl;
                sendto(clientSocket, fullMessage.c_str(), fullMessage.size(), 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
                recvBytes = recvfrom(clientSocket, buffer, BUFFER_SIZE - 1, 0, (struct sockaddr *)&serverAddr, &serverAddrLen);
            } 
        }
        buffer[recvBytes] = '\0';
        cout << Utils::getCurrentTime() + " server " + Utils::addressToString(serverAddr) + buffer << endl;
    }


}
