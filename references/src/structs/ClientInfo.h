#ifndef CLIENTINFO_STRUCT_H
#define CLIENTINFO_STRUCT_H

#include <string>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <netinet/in.h> 
#include <sys/socket.h>
#include "Message.h"

struct ClientInfo {
    std::thread workerThread;
    std::queue<Message> messageQueue;
    std::condition_variable condition_variable;
    std::mutex queueMutex;
    int lastReq;
    int lastSum;
};


#endif // CLIENTINFO_STRUCT_H
