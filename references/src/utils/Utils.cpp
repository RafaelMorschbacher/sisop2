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

std::string Utils::addressToString(const sockaddr_in &addr)
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

std::string Utils::getCurrentTime()
{
    char timeBuffer[80];
    time_t now = time(0);
    struct tm tstruct;
    tstruct = *localtime(&now);
    std::strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %X", &tstruct);
    return std::string(timeBuffer);
}