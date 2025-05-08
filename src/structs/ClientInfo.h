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
using namespace std;

struct ClientInfo {
    thread workerThread;
    queue<Message> messageQueue;
    std::condition_variable condition_variable;
    mutex queueMutex;
    int lastReq;
    int lastSum;
};


#endif // CLIENTINFO_STRUCT_H
