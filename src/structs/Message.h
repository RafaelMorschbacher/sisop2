#ifndef MESSAGE_STRUCT_H
#define MESSAGE_STRUCT_H

#include <string>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <netinet/in.h>
#include <sys/socket.h>

struct Message {
    std::string content;
    sockaddr_in clientAddr;
    socklen_t clientAddrLen;
};


#endif
