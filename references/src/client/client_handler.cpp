#include <iostream>
#include <cstring>
#include <regex>
#include <arpa/inet.h>
#include <unistd.h>

#define BROADCAST_MESSAGE "DISCOVER_SERVER"
#define EXIT_MESSAGE "exit"
#define ACKNOWLEDGEDMESSAGE "acknowledged"
const int BUFFER_SIZE = 1024;

std::string addressToString(const sockaddr_in &addr)
{
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(addr.sin_addr), ip, INET_ADDRSTRLEN);
    return std::string(ip) + ":" + std::to_string(ntohs(addr.sin_port));
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <broadcast port>" << std::endl;
        return 1;
    }

    int broadcastPort = std::atoi(argv[1]);
    if (broadcastPort <= 0)
    {
        std::cerr << "Invalid port number" << std::endl;
        return 1;
    }

    int clientSocket;
    sockaddr_in broadcastAddr, serverAddr;
    char buffer[BUFFER_SIZE];
    int requestId = 0;

    // Create a UDP socket
    clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (clientSocket < 0)
    {
        std::cerr << "Error creating socket." << std::endl;
        return -1;
    }

    // Enable broadcast
    int broadcastEnable = 1;
    if (setsockopt(clientSocket, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)) < 0)
    {
        std::cerr << "ERROR enabling broadcast" << std::endl;
        close(clientSocket);
        return 1;
    }

    // Set broadcast address
    memset(&broadcastAddr, 0, sizeof(broadcastAddr));
    broadcastAddr.sin_family = AF_INET;
    broadcastAddr.sin_port = htons(broadcastPort);
    broadcastAddr.sin_addr.s_addr = htonl(INADDR_BROADCAST);

    // Send broadcast message
    int broadcastReturn = sendto(clientSocket, BROADCAST_MESSAGE, strlen(BROADCAST_MESSAGE), 0, (struct sockaddr *)&broadcastAddr, sizeof(broadcastAddr));
    if (broadcastReturn < 0)
    {
        std::cerr << "ERROR sending broadcast message" << std::endl;
        close(clientSocket);
        return 1;
    }

    socklen_t serverAddrLen = sizeof(serverAddr);
    int recvBytes = recvfrom(clientSocket, buffer, BUFFER_SIZE - 1, 0, (struct sockaddr *)&serverAddr, &serverAddrLen);
    if (recvBytes < 0)
    {
        std::cerr << "Error receiving response from server." << std::endl;
        close(clientSocket);
        return -1;
    }

    // Test "acknowledged" message received from server
    buffer[recvBytes] = '\0';
    if (std::strcmp(buffer, ACKNOWLEDGEDMESSAGE) == 0) {
        std::cout << "Handshake succesfully performed with server." << std::endl;
        std::cout << "Server address: " << addressToString(serverAddr) << std::endl; 
    }
    else {
        std::cout << "Handshake with server failed." << std::endl;
        return -1;
    }

    // Main loop to send messages to the server
    std::string message;
    while (true)
    {
        std::cout << "Enter a number to send to the server (or 'exit' to disconnect): ";
        std::getline(std::cin, message);

        // Check if the user wants to exit or the input stream is closed
        if (std::cin.eof() or message == EXIT_MESSAGE)
        {
            // Send exit message to the server  and break the loop
            std::cout << "Disconnecting from server..." << std::endl;
            sendto(clientSocket, EXIT_MESSAGE, 4, 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
            break;
        }

        std::string fullMessage = std::to_string(++requestId) + " " + message;

        sendto(clientSocket, fullMessage.c_str(), fullMessage.size(), 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

        recvBytes = recvfrom(clientSocket, buffer, BUFFER_SIZE - 1, 0, (struct sockaddr *)&serverAddr, &serverAddrLen);
        if (recvBytes < 0)
        {
            std::cerr << "Error receiving response from server." << std::endl;
            continue;
        }
        buffer[recvBytes] = '\0';
        std::cout << "Server response: " << buffer << std::endl;
    }

    close(clientSocket);
    return 0;
}
