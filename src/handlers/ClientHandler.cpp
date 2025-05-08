#include "ClientHandler.h"
#include "Utils.h"
#include <unistd.h>

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

    
struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 10000; // 10 milliseconds = 10,000 microseconds

    if (setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) 
    {
        perror("setsockopt SO_RCVTIMEO failed");
        close(clientSocket);
        return 1;
    }

    int broadcastEnable = 1;
    if (setsockopt(clientSocket, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)) > 0)
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
        //2024-10-01 18:37:00 server_addr 1.1.1.20
        std::cout << Utils::getCurrentTime() + " server_addr " +  Utils::addressToString(serverAddr) << std::endl;
        //std::cout << "Endereço servidor: " << Utils::addressToString(serverAddr) << std::endl;
        // 2024-10-01 18:37:01 client 1.1.1.2  <MANDAR SÓ A PARTE DA DIREITA -> id_req 1 value 10 num_reqs 1 total_sum 1
        //std::cout << "Handshake com o servidor com sucesso" << std::endl;
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
        // std::cout << "Informe um número para enviar ao servidor (ou 'exit' para sair): ";
        
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

            if (errno == EAGAIN || errno == EWOULDBLOCK) 
            {
                std::cerr << "Erro ao receber resposta do servidor, iniciando retransmissão depois de 10 milisegundos." << std::endl;
                sleep(5);
                sendto(clientSocket, fullMessage.c_str(), fullMessage.size(), 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
                recvBytes = recvfrom(clientSocket, buffer, BUFFER_SIZE - 1, 0, (struct sockaddr *)&serverAddr, &serverAddrLen);
            } 
        }
        buffer[recvBytes] = '\0';
        std::cout << Utils::getCurrentTime() + " server " + Utils::addressToString(serverAddr) + buffer << std::endl;
    }


}
