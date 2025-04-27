#include <iostream>
#include <cstring>
#include <regex>
#include <arpa/inet.h>
#include <unistd.h>
#include "Global.h"
#include "Utils.h"

class ClientHandler
{
private:
    int clientSocket;
    sockaddr_in broadcastAddr, serverAddr;
    char buffer[BUFFER_SIZE];
    int requestId;

public:
    ClientHandler() : clientSocket(-1), requestId(0) {}

    ~ClientHandler()
    {
        if (clientSocket >= 0)
        {
            close(clientSocket);
        }
    }

    bool initializeSocket(int broadcastPort)
    {
        // Criação de socket UDP
        clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
        if (clientSocket < 0)
        {
            std::cerr << "Erro ao criar socket" << std::endl;
            return false;
        }

        // Ativa o socket para permitir broadcast
        int broadcastEnable = 1;
        if (setsockopt(clientSocket, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)) < 0)
        {
            std::cerr << "Erro ao habilitar socket" << std::endl;
            close(clientSocket);
            return false;
        }

        // Configura o endereço do servidor para o broadcast
        memset(&broadcastAddr, 0, sizeof(broadcastAddr));
        broadcastAddr.sin_family = AF_INET;
        broadcastAddr.sin_port = htons(broadcastPort);
        broadcastAddr.sin_addr.s_addr = htonl(INADDR_BROADCAST);

        return true;
    }

    bool performHandshake()
    {
        // Envia mensagem de broadcast para o servidor
        int broadcastReturn = sendto(clientSocket, BROADCAST_MESSAGE, strlen(BROADCAST_MESSAGE), 0, (struct sockaddr *)&broadcastAddr, sizeof(broadcastAddr));
        if (broadcastReturn < 0)
        {
            std::cerr << "Erro ao enviar mensagem de broadcast" << std::endl;
            return false;
        }

        // Espera pela resposta do servidor
        socklen_t serverAddrLen = sizeof(serverAddr);
        int recvBytes = recvfrom(clientSocket, buffer, BUFFER_SIZE - 1, 0, (struct sockaddr *)&serverAddr, &serverAddrLen);
        if (recvBytes < 0)
        {
            std::cerr << "Erro ao receber resposta do servidor." << std::endl;
            return false;
        }

        // Verifica mensagem de acknowledgment
        buffer[recvBytes] = '\0';
        if (std::strcmp(buffer, ACKNOWLEDGEDMESSAGE) == 0)
        {
            std::cout << "Handshake com o servidor com sucesso" << std::endl;
            std::cout << "Endereço servidor: " << Utils::addressToString(serverAddr) << std::endl;
            return true;
        }
        else
        {
            std::cout << "Handshake com o servidor falhou" << std::endl;
            return false;
        }
    }

    void mainLoop()
    {
        std::string message;
        while (true)
        {
            std::cout << "Informe um número para enviar ao servidor (ou 'exit' para sair): ";
            std::getline(std::cin, message);

            // Verifica se o usuario quer sair ou se o input stream terminou
            if (std::cin.eof() or message == EXIT_MESSAGE)
            {
                // Envia mensagem de desconexão
                std::cout << "Desconectando do servidor..." << std::endl;
                sendto(clientSocket, EXIT_MESSAGE, 4, 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
                break;
            }

            std::string fullMessage = std::to_string(++requestId) + " " + message;

            sendto(clientSocket, fullMessage.c_str(), fullMessage.size(), 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

            socklen_t serverAddrLen = sizeof(serverAddr);
            int recvBytes = recvfrom(clientSocket, buffer, BUFFER_SIZE - 1, 0, (struct sockaddr *)&serverAddr, &serverAddrLen);
            if (recvBytes < 0)
            {
                std::cerr << "Erro ao receber resposta do servidor." << std::endl;
                continue;
            }
            buffer[recvBytes] = '\0';
            std::cout << "Resposta do servidor: " << buffer << std::endl;
        }
    }
};

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
