#include <netinet/in.h>
#include <string>
#ifndef UTILS_H
#define UTILS_H

class Utils {
    public:
        static std::string addressToString(const sockaddr_in &addr);

        static std::string getCurrentTime();
};

#endif
