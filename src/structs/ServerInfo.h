#ifndef SERVERINFO_STRUCT_H
#define SERVERINFO_STRUCT_H

#include <string>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <netinet/in.h>
#include <sys/socket.h>

struct ServerInfo {
    int num_reqs;
    int total_sum;
};


#endif
