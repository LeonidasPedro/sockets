#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define MAX_CLIENTS 100
#define BUFFER_SIZE 1024
#define EXIT_COMMAND "exit"

int client_sockets[MAX_CLIENTS];
int client_count = 0;

void broadcast_message(char *message) {
    for (int i = 0; i < client_count; i++) {
        send(client_sockets[i], message, strlen(message), 0);
    }
}

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received <= 0 || strcmp(buffer, EXIT_COMMAND) == 0) {
            break;
        }
        broadcast_message(buffer);
    }
    close(client_socket);
}

int main() {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8000);
    server_address.sin_addr.s_addr = INADDR_ANY;

    bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address));
    listen(server_socket, MAX_CLIENTS);

    while (1) {
        int client_socket = accept(server_socket, NULL, NULL);
        client_sockets[client_count++] = client_socket;
        handle_client(client_socket);
    }

    return 0;
}

