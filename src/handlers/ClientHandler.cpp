#include "ClientHandler.h"

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
        std::cerr << "Erro ao criar socket" << std::endl;
        return false;
    }

    int broadcastEnable = 1;
    if (setsockopt(clientSocket, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)) < 0)
    {
        std::cerr << "Erro ao habilitar socket" << std::endl;
        close(clientSocket);
        return false;
    }

    memset(&broadcastAddr, 0, sizeof(broadcastAddr));
    broadcastAddr.sin_family = AF_INET;
    broadcastAddr.sin_port = htons(broadcastPort);
    broadcastAddr.sin_addr.s_addr = htonl(INADDR_BROADCAST);

    return true;
}

bool ClientHandler::performHandshake()
{
    int broadcastReturn = sendto(clientSocket, BROADCAST_MESSAGE, strlen(BROADCAST_MESSAGE), 0, (struct sockaddr *)&broadcastAddr, sizeof(broadcastAddr));
    if (broadcastReturn < 0)
    {
        std::cerr << "Erro ao enviar mensagem de broadcast" << std::endl;
        return false;
    }

    socklen_t serverAddrLen = sizeof(serverAddr);
    int recvBytes = recvfrom(clientSocket, buffer, BUFFER_SIZE - 1, 0, (struct sockaddr *)&serverAddr, &serverAddrLen);
    if (recvBytes < 0)
    {
        std::cerr << "Erro ao receber resposta do servidor." << std::endl;
        return false;
    }

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

void ClientHandler::mainLoop()
{
    std::string message;
    while (true)
    {
        std::cout << "Informe um número para enviar ao servidor (ou 'exit' para sair): ";
        std::getline(std::cin, message);

        if (std::cin.eof() || message == EXIT_MESSAGE)
        {
            std::cout << "Desconectando do servidor..." << std::endl;
            sendto(clientSocket, EXIT_MESSAGE, 4, 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
            break;
        }

        std::string fullMessage = std::to_string(++requestId) + " " + message;

        sendto(clientSocket, fullMessage.c_str(), fullMessage.size(), 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

        socklen_t serverAddrLen = sizeof(serverAddr);
        int recvBytes = recvfrom(clientSocket, buffer, BUFFER_SIZE - 1, 0, (struct sockaddr *)&serverAddr, &serverAddrLen);
        while(recvBytes < 0)
        {
            std::cerr << "Erro ao receber resposta do servidor, iniciando retransmissão." << std::endl;
            sendto(clientSocket, fullMessage.c_str(), fullMessage.size(), 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
            recvBytes = recvfrom(clientSocket, buffer, BUFFER_SIZE - 1, 0, (struct sockaddr *)&serverAddr, &serverAddrLen);
        }
        // if (recvBytes < 0)
        // {
        //     std::cerr << "Erro ao receber resposta do servidor, iniciando retransmissão." << std::endl;
            
        // }
        buffer[recvBytes] = '\0';
        std::cout << buffer << std::endl;
    }
}
