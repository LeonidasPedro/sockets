#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define MAX_CLIENTS 8

void error(const char *msg) {
    perror(msg);
    exit(1);
}

void *handle_client(void *arg) {
    int client_socket = *((int *)arg);
    char buffer[256];
    int n;

    while (1) {
        bzero(buffer, 256);
        n = read(client_socket, buffer, 255);
        if (n < 0) {
            error("Erro ao ler do socket");
        }

        printf("Cliente %d: %s\n", client_socket, buffer);
    }

    close(client_socket);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Porta nao fornecida. Encerrando...\n");
        exit(1);
    }

    int sockfd, newsockfd, portno;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;

    int client_sockets[MAX_CLIENTS]; // Array para armazenar os descritores de socket dos clientes
    int num_clients = 0;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error("Erro ao abrir o socket");

    bzero((char *)&serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("Erro ao fazer o bind");

    listen(sockfd, MAX_CLIENTS); // Defina o número máximo de conexões para 8

    printf("Aguardando conexões...\n");

    while (1) {
        clilen = sizeof(cli_addr);

        // Aceite a conexão
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
        if (newsockfd < 0) {
            error("Erro ao aceitar a conexao");
        }

        printf("Novo cliente conectado.\n");

        // Adicione o novo descritor de socket ao array
        if (num_clients < MAX_CLIENTS) {
            client_sockets[num_clients] = newsockfd;

            // Inicie uma nova thread para lidar com o cliente
            pthread_t client_thread;
            if (pthread_create(&client_thread, NULL, handle_client, (void *)&client_sockets[num_clients]) != 0) {
                fprintf(stderr, "Erro ao criar thread para o cliente.\n");
                close(newsockfd);
            } else {
                num_clients++;
            }
        } else {
            printf("Limite máximo de clientes atingido. Novas conexões serão aceitas, mas não adicionadas ao array.\n");
            // Note que a conexão ainda é aceita, mas não adicionada ao array
            close(newsockfd);
        }

        // Continue aceitando novas conexões e lidando com os clientes nas threads...
    }

    // Não alcançará este ponto, mas seria necessário fechar o socket principal
    close(sockfd);

    return 0;
}
