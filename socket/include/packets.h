#include <cstdint>

// Structs dadas pelo professor para o trabalho

struct requisicao {
    uint32_t value; // Valor da requsição
};

struct requisicao_ack {
    uint32_t seqn; // Número de sequência que está sendo feito o ack
    uint32_t num_reqs; // Quantidade de requisições
    uint64_t total_sum; // Valor da soma agregada até o momento
};

typedef struct __packet {
    uint16_t type; // Tipo do pacote (DESC | REQ | DESC_ACK | REQ_ACK )
    uint32_t seqn; // Número de sequência de uma requisição
    union {
        struct requisicao req;
        struct requisicao_ack ack;
    };
} packet;
