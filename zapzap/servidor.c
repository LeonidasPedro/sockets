#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

void error(const char *msg){
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    if (argc < 2){
        fprintf(stderr, "Porta nao fornecida. Encerrando...\n");
        exit(1);
    }

    int sockfd, newsockfd, portno, n;  
    char buffer[256];

    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error("Erro ao abrir o socket");

    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    serv_addr.sin_port = htons(portno);

    if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("Erro ao fazer o bind");
    
    listen(sockfd, 5);

    clilen = sizeof(cli_addr);

    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

    if (newsockfd < 0) error("Erro ao aceitar a conexao");

    while (1){
        bzero(buffer, 256);
        n = read(newsockfd, buffer, 255);

        if (n < 0) error("Erro ao ler do socket");

        printf("Cliente: %s\n", buffer);

        bzero(buffer, 256);
        fgets(buffer, 255, stdin);

        n = write(newsockfd, buffer, strlen(buffer));

        if (n < 0) error("Erro ao escrever no socket");

        int i = strncmp("Tchau", buffer, 5);

        if (i == 0) break;

    }

    close(newsockfd);
    close(sockfd);

    return 0;
}