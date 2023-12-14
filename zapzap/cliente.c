#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

void error(const char *msg)
{
  perror(msg);
  exit(0);
}

void handle_received_message(int sender, const char *message)
{
  if (strncmp(message, "global:", 7) == 0)
  {
    printf("Global: %s", message + 7);
  }
  else if (strncmp(message, "private:", 8) == 0)
  {
    int recipient_index;
    char private_message[256];
    if (sscanf(message, "private:%d:%s", &recipient_index, private_message) == 2)
    {
      printf("Mensagem privada de Cliente %d para Cliente %d: %s\n", sender, recipient_index, private_message + 2);
    }
    else
    {
      printf("Erro ao processar mensagem privada.\n");
    }
  }
  else
  {
    printf("Mensagem desconhecida: %s", message);
  }
}

int main(int argc, char *argv[])
{
  int sockfd, portno, n;
  struct sockaddr_in serv_addr;
  struct hostent *server;

  char buffer[256];
  fd_set readfds;
  struct timeval tv;

  if (argc < 3)
  {
    fprintf(stderr, "usage %s hostname port\n", argv[0]);
    exit(0);
  }

  portno = atoi(argv[2]);

  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  if (sockfd < 0)
    error("ERROR opening socket");

  server = gethostbyname(argv[1]);

  if (server == NULL)
  {
    fprintf(stderr, "ERROR, no such host\n");
    exit(0);
  }
  printf("Cliente: ");

  bzero((char *)&serv_addr, sizeof(serv_addr));

  serv_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
  serv_addr.sin_port = htons(portno);

  if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    error("ERROR connecting");

  while (1)
  {
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);
    FD_SET(sockfd, &readfds);

    tv.tv_sec = 0;
    tv.tv_usec = 0;

    if (select(sockfd + 1, &readfds, NULL, NULL, &tv) > 0)
    {
      if (FD_ISSET(STDIN_FILENO, &readfds))
      {
        bzero(buffer, 256);
        fgets(buffer, 255, stdin);
        n = write(sockfd, buffer, strlen(buffer));

        if (n < 0)
          error("ERROR writing to socket");
      }

      if (FD_ISSET(sockfd, &readfds))
      {
        bzero(buffer, 256);
        n = read(sockfd, buffer, 255);

        if (n < 0)
          error("ERROR reading from socket");

        handle_received_message(sockfd, buffer);

        int i = strncmp("Bye", buffer, 3);

        if (i == 0)
          break;
      }
    }
  }

  close(sockfd);

  return 0;
}
