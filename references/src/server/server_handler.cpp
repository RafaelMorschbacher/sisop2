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

#define BROADCAST_MESSAGE "DISCOVER_SERVER"
#define EXIT_MESSAGE "exit"
#define ACKNOWLEDGEDMESSAGE "acknowledged"
const int BUFFER_SIZE = 1024;

struct Message
{
    std::string content;
    sockaddr_in clientAddr;
    socklen_t clientAddrLen;
};

struct ClientInfo
{
    std::thread workerThread;
    std::queue<Message> messageQueue;
    std::condition_variable condition_variable;
    std::mutex queueMutex;
    int lastReq;
    int lastSum;
};

struct ServerInfo
{
    int num_reqs;
    int total_sum;
};

ServerInfo serverInfo = {0, 0};
std::unordered_map<std::string, ClientInfo> clients;
std::mutex globalVarMutex;
std::mutex clientsMutex;
int globalSum = 0;

/**
 * @brief Converts a sockaddr_in structure to a human-readable string.
 *
 * This function takes a sockaddr_in structure, extracts the IP address and port,
 * and converts them into a string in the format "IP:port".
 *
 * @param addr The sockaddr_in structure containing the IP address and port.
 * @return A string representation of the IP address and port in the format "IP:port".
 */
std::string addressToString(const sockaddr_in &addr)
{
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(addr.sin_addr), ip, INET_ADDRSTRLEN);
    return std::string(ip) + ":" + std::to_string(ntohs(addr.sin_port));
}

/**
 * @brief Retrieves the current local time as a formatted string.
 *
 * This function gets the current local time and formats it into a string
 * in the format "YYYY-MM-DD HH:MM:SS".
 *
 * @return A string representing the current local time.
 */

std::string getCurrentTime()
{
    char timeBuffer[80];
    time_t now = time(0);
    struct tm tstruct;
    tstruct = *localtime(&now);
    std::strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %X", &tstruct);
    return std::string(timeBuffer);
}

/**
 * @brief Processes messages from a client in a loop until the message queue is empty.
 *
 * This function continuously waits for messages from a specific client, processes them,
 * and sends appropriate responses back to the client. It updates the global sum and server
 * information based on the client's requests.
 *
 * @param serverSocket The socket descriptor for the server.
 * @param clientKey The unique key identifying the client.
 *
 *
 * The function uses mutexes to ensure thread safety when accessing shared resources
 * such as the client's message queue, global sum, and server information.
 */
void processMessage(int serverSocket, const std::string &clientKey)
{
    ClientInfo &clientInfo = clients[clientKey];

    while (true)
    {
        // Wait for a message in the queue to process
        {
            std::unique_lock<std::mutex> lock(clientInfo.queueMutex);
            clientInfo.condition_variable.wait(lock, [&clientInfo]()
                                               { return !clientInfo.messageQueue.empty(); });

            if (clientInfo.messageQueue.empty())
            {
                break;
            }
        }

        if (!clientInfo.messageQueue.empty())
        {
            Message msg;

            // get the top message from the queue
            {
                std::lock_guard<std::mutex> lock(clientInfo.queueMutex);
                msg = clientInfo.messageQueue.front();
                clientInfo.messageQueue.pop();
            }

            std::string response;
            std::istringstream iss(msg.content);
            int requestId, clientNumber;
            iss >> requestId >> clientNumber;

            if (iss.fail() || !iss.eof())
            {
                response = "Invalid message format.";
                clientInfo.lastReq = requestId;
            }
            else
            {
                // Check if the request ID is the expected one
                if (requestId == clientInfo.lastReq + 1)
                {
                    clientInfo.lastReq = requestId;

                    // Update global sum and number of requests with a mutex
                    {
                        std::lock_guard<std::mutex> lock(globalVarMutex);
                        globalSum += clientNumber;
                        clientInfo.lastSum = globalSum;
                        serverInfo.num_reqs++;
                        serverInfo.total_sum = globalSum;
                    }


                    response = "Date " + getCurrentTime() + " - Number of requests: " + std::to_string(serverInfo.num_reqs) + " - Total accumulated: " + std::to_string(globalSum) + " (Thread ID: " + std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id())) + ")";
                }
                else
                {
                    response = "Incorrect request number.";
                }
            }
            ssize_t sentBytes = sendto(serverSocket, response.c_str(), response.size(), 0,
                                       (struct sockaddr *)&msg.clientAddr, msg.clientAddrLen);
            if (sentBytes < 0)
            {
                std::cerr << "Error sending response to client " << clientKey << "." << std::endl;
            }
            else
            {
                std::cout << "Response sent to client " << clientKey << ": " << response << std::endl;
            }
        }
    }

    {
        std::lock_guard<std::mutex> clientsLock(clientsMutex);
        clients.erase(clientKey);
    }
}

/**
 * @brief Handles the discovery message from a client.
 *
 * This function processes a discovery message received from a client. It checks if the client is already connected,
 * and if not, it creates a new entry for the client and starts a new thread to process its messages. It also retrieves
 * the server port and sends it back to the client.
 *
 * @param serverSocket The socket file descriptor for the server.
 * @param clientAddr The address of the client sending the discovery message.
 * @param clientAddrLen The length of the client address structure.
 */
void handleDiscoveryMessage(int serverSocket, sockaddr_in clientAddr, socklen_t clientAddrLen)
{
    FILE *file = fopen("src/log_log.log", "w");
    std::string clientKey = addressToString(clientAddr);

    std::lock_guard<std::mutex> lock(clientsMutex);
    if (clients.find(clientKey) != clients.end())
    {
        std::cout << "Client " << clientKey << " is already connected." << std::endl;
        return;
    }

    // If the client is not already connected, create a new entry for it and a new thread to process its messages
    ClientInfo &clientInfo = clients[clientKey];
    clientInfo.lastReq = 0;
    clientInfo.lastSum = 0;
    clientInfo.workerThread = std::thread(processMessage, serverSocket, clientKey);

    // Send the handshake to the client
    ssize_t sentBytes = sendto(serverSocket, ACKNOWLEDGEDMESSAGE, 12, 0, (struct sockaddr *)&clientAddr, clientAddrLen);
    if (sentBytes < 0)
    {
        std::cerr << "Error sending acknowledge message to client " << clientKey << "." << std::endl;
        fprintf(file, "Error sending acknowledge message to client %s.\n", clientKey.c_str());
    }
    else
    {
        std::cout << "Handshake sent to client " << clientKey << "." << std::endl;
        fprintf(file, "Handshake sent to client %s.\n", clientKey.c_str());
    }
}

/**
 * @brief Handles the exit message from a client.
 *
 * This function is called when a client sends an exit message. It converts the client's address to a string,
 * locks the clients mutex to ensure thread safety, and checks if the client is in the clients map. If the client
 * is found, it notifies all threads waiting on the client's condition variable and logs the disconnection message.
 * If the client is not found, it logs that the client was not found.
 *
 * @param serverSocket The server socket descriptor.
 * @param clientAddr The address of the client.
 * @param clientAddrLen The length of the client address.
 */
void handleExitMessage(int serverSocket, sockaddr_in clientAddr, socklen_t clientAddrLen)
{
    std::string clientKey = addressToString(clientAddr);

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

int main(int argc, char *argv[])
{
    int serverSocket;
    sockaddr_in serverAddr, clientAddr;
    char buffer[BUFFER_SIZE];

    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
        return 1;
    }

    int port = std::atoi(argv[1]);
    if (port <= 0)
    {
        std::cerr << "Invalid port number" << std::endl;
        return 1;
    }

    // Create a UDP socket
    serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (serverSocket < 0)
    {
        std::cerr << "Error creating server socket." << std::endl;
        return -1;
    }

    // Set server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);


    // Bind the created socket to an address and port
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        std::cerr << "Error binding socket to address." << std::endl;
        close(serverSocket);
        return -1;
    }

    std::cout << "UDP server started on port " << port << "." << std::endl;
    std::cout << "Main thread ID: " << std::this_thread::get_id() << "." << std::endl;
    std::cout << "Current sum: " << std::to_string(globalSum) << std::endl;
    std::cout << "Total requests: " << std::to_string(serverInfo.num_reqs) << std::endl;

    // Main loop to receive messages from clients
    while (true)
    {
        socklen_t clientAddrLen = sizeof(clientAddr);
        int recvBytes = recvfrom(serverSocket, buffer, BUFFER_SIZE - 1, 0,
                                 (struct sockaddr *)&clientAddr, &clientAddrLen);
        if (recvBytes <= 0)
        {
            std::cerr << "Error receiving data." << std::endl;
            continue;
        }

        buffer[recvBytes] = '\0';
        std::string message(buffer);

        // Check if the message is a discovery message
        if (std::strcmp(message.c_str(), BROADCAST_MESSAGE) == 0)
        {
            handleDiscoveryMessage(serverSocket, clientAddr, clientAddrLen);
            continue;
        }

        // Check if the message is an exit message
        if (std::strcmp(message.c_str(), EXIT_MESSAGE) == 0)
        {
            handleExitMessage(serverSocket, clientAddr, clientAddrLen);
            continue;
        }

        // If the message is not a discovery or exit message, process it
        std::string clientKey = addressToString(clientAddr);
        {
            std::lock_guard<std::mutex> lock(clientsMutex);
            auto it = clients.find(clientKey);
            if (it != clients.end())
            {
                {
                    std::lock_guard<std::mutex> queueLock(it->second.queueMutex);
                    it->second.messageQueue.push(Message{message, clientAddr, clientAddrLen});
                }
                it->second.condition_variable.notify_one();
            }
            else
            {
                std::cout << "Message received from unregistered client: " << clientKey << "." << std::endl;
            }
        }
    }

    close(serverSocket);
    return 0;
}
