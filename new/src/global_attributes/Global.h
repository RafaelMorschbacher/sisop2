// Globals.h
#ifndef GLOBAL_H
#define GLOBAL_H

#include <string>
#include <unordered_map>
#include <mutex>
#include "ServerInfo.h"
#include "ClientInfo.h"

constexpr const char* BROADCAST_MESSAGE = "DISCOVER_SERVER";
constexpr const char* EXIT_MESSAGE = "exit";
constexpr const char* ACKNOWLEDGEDMESSAGE = "acknowledged";
constexpr int BUFFER_SIZE = 1024;

extern ServerInfo serverInfo;
extern std::unordered_map<std::string, ClientInfo> clients;
extern std::mutex globalVarMutex;
extern std::mutex clientsMutex;
extern int globalSum;

#endif
