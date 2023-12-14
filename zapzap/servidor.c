#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define MAX_CLIENTS 8

int client_sockets[MAX_CLIENTS];

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void send_to_user(int sender, int recipient, const char *message)
{
    char formatted_message[255 + 20];
    snprintf(formatted_message, sizeof(formatted_message), "private:%d:%s", recipient, message);

    if (client_sockets[recipient] > 0)
    {
        int n = write(client_sockets[recipient], formatted_message, strlen(formatted_message));
        if (n < 0)
        {
            error("Erro ao escrever no socket");
        }
    }
}

void broadcast(int sender, const char *message)
{
    char formatted_message[255 + 10];
    snprintf(formatted_message, sizeof(formatted_message), "global:%s", message);

    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (i != sender && client_sockets[i] > 0)
        {
            int n = write(client_sockets[i], formatted_message, strlen(formatted_message));
            if (n < 0)
            {
                error("Erro ao escrever no socket");
            }
        }
    }
}

void *handle_client(void *arg)
{
    int client_socket = *((int *)arg);
    char buffer[256];
    int n;

    while (1)
    {
        bzero(buffer, 256);
        n = read(client_socket, buffer, 255);
        if (n < 0)
        {
            error("Erro ao ler do socket");
        }

        if (n == 0)
        {
    
            printf("Cliente %d desconectado.\n", client_socket);
            break;
        }

        printf("Cliente %d: %s\n", client_socket, buffer);

        
        if (strncmp(buffer, "private:", 8) == 0)
        {
            int recipient;
            if (sscanf(buffer + 8, "%d:", &recipient) == 1)
            {
                
                send_to_user(client_socket, recipient, buffer + 8);
            }
        }
        else
        {
           
            broadcast(client_socket, buffer);
        }
    }

    close(client_socket);
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Porta nao fornecida. Encerrando...\n");
        exit(1);
    }

    int sockfd, newsockfd, portno;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;

    
    int num_clients = 0;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("Erro ao abrir o socket");

    bzero((char *)&serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("Erro ao fazer o bind");

    listen(sockfd, MAX_CLIENTS); 

    printf("Aguardando conexões...\n");

    while (1)
    {
        clilen = sizeof(cli_addr);

        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
        if (newsockfd < 0)
        {
            error("Erro ao aceitar a conexao");
        }

        printf("Novo cliente conectado.\n");

        if (num_clients < MAX_CLIENTS)
        {
            client_sockets[num_clients] = newsockfd;

            
            pthread_t client_thread;
            if (pthread_create(&client_thread, NULL, handle_client, (void *)&client_sockets[num_clients]) != 0)
            {
                fprintf(stderr, "Erro ao criar thread para o cliente.\n");
                close(newsockfd);
            }
            else
            {
                num_clients++;
            }
        }
        else
        {
            printf("Limite máximo de clientes atingido. Novas conexões serão aceitas, mas não adicionadas ao array.\n");
            close(newsockfd);
        }
    }

    close(sockfd);

    return 0;
}
