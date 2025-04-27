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
#include "Utils.h"

// Função que converte um endereço IP e porta em uma string no formato "IP:PORT".
std::string Utils::addressToString(const sockaddr_in &addr)
{
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(addr.sin_addr), ip, INET_ADDRSTRLEN);
    return std::string(ip) + ":" + std::to_string(ntohs(addr.sin_port));
}

 // Função que obtém a hora local atual em uma string no formato "YYYY-MM-DD HH:MM:SS".
std::string Utils::getCurrentTime()
{
    char timeBuffer[80];
    time_t now = time(0);
    struct tm tstruct;
    tstruct = *localtime(&now);
    std::strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %X", &tstruct);
    return std::string(timeBuffer);
}