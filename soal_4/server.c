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
    int serverSocket;
    struct sockaddr_in serverAddress;
    char *buffer;
    size_t len = 0;
    ssize_t read;
    char *token;
    char *animeData;
    char *animeTitle;
    char *animeGenre;
    char *animeStatus;
    char *animeDate;
    char *animeType;
    char *animeMessage;
    char *logMessage;
    char *logDate;
    char *logType;
    char *logMassage;

    // Membuat socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        perror("Error creating socket");
        return -1;
    }

    // Mengatur alamat server
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr);

    // Membuka file myanimelist.csv
    FILE *file = fopen("mynamelist.csv", "r");
    if (file == NULL) {
        perror("Error opening file");
        return -1;
    }

    // Membuat socket listen
    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("Error binding socket");
        return -1;
    }

    if (listen(serverSocket, 3) < 0) {
        perror("Error listening");
        return -1;
    }

    printf("Server listening on port %d...\n", PORT);

    while (1) {
        int clientSocket;
        struct sockaddr_in clientAddress;
        socklen_t clientAddressLength = sizeof(clientAddress);

        // Menerima koneksi dari client
        clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientAddressLength);
        if (clientSocket < 0) {
            perror("Error accepting connection");
            continue;
        }

        printf("Client connected...\n");

        while (1) {
            // Menerima perintah dari client
            buffer = malloc(BUFFER_SIZE);
            read = getline(&buffer, &len, stdin);
            send(clientSocket, buffer, read, 0);

            // Menerima respon dari client
            read = getline(&buffer, &len, stdin);
            printf("Client response: %s\n", buffer);

            // Jika user meminta keluar
            if (strcmp(buffer, "exit\n") == 0) {
                break;
            }
        }

        // Menutup socket
        close(clientSocket);
    }

    // Menutup file
    fclose(file);

    return 0;
}
