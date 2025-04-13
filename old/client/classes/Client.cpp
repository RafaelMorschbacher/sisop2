#include "UdpClient.h"
class Client{
    public:
        Client(std::string serverIP, int serverPort) 
            : udpClientSocket(serverIP, serverPort) {
               // udpClientSocket.initialize();
                
        };
    private:
        UdpClient udpClientSocket;
};