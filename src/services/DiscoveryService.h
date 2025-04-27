#include <netinet/in.h>
#ifndef DISCOVERYSERVICE_H
#define DISCOVERYSERVICE_H

// Essa classe lida com mensagens de descoberta de clientes. Verifica se o cliente já está conectado,
// caso contrário, cria uma nova entrada para o cliente e inicia uma nova thread para processar suas mensagens.
// Também recupera a porta do servidor e a envia de volta para o cliente.
class DiscoveryService {
    public:
        
        static void handleDiscoveryMessage(int serverSocket, sockaddr_in clientAddr, socklen_t clientAddrLen);
};

#endif