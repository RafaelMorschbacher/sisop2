#include "Global.h"

ServerInfo serverInfo = {0, 0};
std::unordered_map<std::string, ClientInfo> clients;
std::mutex globalVarMutex;
std::mutex clientsMutex;
int globalSum = 0;
