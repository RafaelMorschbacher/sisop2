#include <netinet/in.h>
#ifndef INTERFACESERVICE_H
#define INTERFACESERVICE_H

// Essa classe lida com mensagens de saída de clientes.
// Converte o endereço do cliente em uma string, bloqueia o mutex dos clientes para garantir a segurança da thread, 
// verifica se o cliente foi encontrado,  caso sim ela notifica todas as threads que estão esperando na variável de 
// condição do cliente e registra a mensagem de desconexão.
// Caso contrário, registra que o cliente não foi encontrado.
class InterfaceService {
    public:
        // Método estático para lidar com mensagens de saída
        static void handleExitMessage(int serverSocket, sockaddr_in clientAddr, socklen_t clientAddrLen);
};

#endif
