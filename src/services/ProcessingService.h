#include <netinet/in.h>
#include <string>
#ifndef PROCESSINGSERVICE_H
#define PROCESSINGSERVICE_H

// Essa classe continuamente espera por mensagens de um cliente específico, processa essas mensagens 
// e envia respostas apropriadas de volta ao cliente.
// Ela atualiza a soma global e as informações do servidor com base nas solicitações do cliente.
class ProcessingService {
    public:
        // Método estático para processar mensagens de um cliente específico
        static void processMessage(int serverSocket, const std::string &clientKey);
};

#endif
