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
#include <iomanip>
using namespace std;

// Função que converte um endereço IP e porta em uma string no formato "IP:PORT".
string Utils::addressToString(const sockaddr_in &addr)
{
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(addr.sin_addr), ip, INET_ADDRSTRLEN);
    return string(ip);

}

 // Função que obtém a hora local atual em uma string no formato "YYYY-MM-DD HH:MM:SS".
string Utils::getCurrentTime()
{
    time_t now = time(nullptr);
    tm *ltm = localtime(&now);
    ostringstream stream;
    stream << put_time(ltm, "%Y-%m-%d %H:%M:%S");
    return stream.str();
}