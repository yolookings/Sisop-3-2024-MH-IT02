#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int clientSocket;
    struct sockaddr_in serverAddress;
    char buffer[BUFFER_SIZE];
    char command[100];
    char response[100];

    // Membuat socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        perror("Error creating socket");
        return -1;
    }

    // Mengatur alamat server
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr);

    // Koneksi ke server
    if (connect(clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("Error connecting to server");
        return -1;
    }

    while (1) {
        printf("Enter command (list, search, add, edit, delete, exit): ");
        fgets(command, sizeof(command), stdin);

        // Mengirimkan perintah ke server
        send(clientSocket, command, strlen(command), 0);

        // Menerima respon dari server
        recv(clientSocket, response, sizeof(response), 0);
        printf("Server response: %s\n", response);

        // Jika user meminta keluar
        if (strcmp(command, "exit\n") == 0) {
            break;
        }
    }

    // Menutup socket
    close(clientSocket);

    return 0;
}
