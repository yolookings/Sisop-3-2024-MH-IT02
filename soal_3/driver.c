// driver.c

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <time.h>
#include </Users/mwlanaz/desktop/praktikum/praktikum-sisop/modul-3/soal_3/action.h>

#define PORT 8080
#define LOG_FILE "race.log"

// Deklarasi fungsi log_message
void log_message(char* source, char* command, char* additional_info);

// Deklarasi fungsi handle_client
void handle_client(int client_socket);

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s -c [Command] -i [Info]\n", argv[0]);
        return 1;
    }

    char* command = NULL;
    char* info = NULL;

    // Parsing argumen dari command line
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-c") == 0 && i + 1 < argc) {
            command = argv[i + 1];
        } else if (strcmp(argv[i], "-i") == 0 && i + 1 < argc) {
            info = argv[i + 1];
        }
    }

    if (command == NULL || info == NULL) {
        printf("Invalid command format. Usage: %s -c [Command] -i [Info]\n", argv[0]);
        return 1;
    }

    // Membuat socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        return 1;
    }

    // Menyambungkan ke server
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        return 1;
    }

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        return 1;
    }

    // Mengirimkan pesan ke server
    char message[256];
    snprintf(message, sizeof(message), "%s %s", command, info);
    send(sock, message, strlen(message), 0);

    // Menerima respon dari server
    char response[1024] = {0};
    recv(sock, response, sizeof(response), 0);
    printf("%s\n", response);

    close(sock);
    return 0;
}

void log_message(char* source, char* command, char* additional_info) {
    time_t current_time;
    struct tm* time_info;
    char time_str[20];
    
    time(&current_time);
    time_info = localtime(&current_time);
    strftime(time_str, sizeof(time_str), "%d/%m/%Y %H:%M:%S", time_info);
    
    FILE* log_file = fopen(LOG_FILE, "a");
    if (log_file != NULL) {
        fprintf(log_file, "[%s] [%s]: [%s] [%s]\n", source, time_str, command, additional_info);
        fclose(log_file);
    }
}

void handle_client(int client_socket) {
    char buffer[1024] = {0};
    int valread;
    char response[1024] = {0};
    
    // Kirim pesan selamat datang ke driver.c
    char* welcome_message = "Welcome to Paddock!";
    send(client_socket, welcome_message, strlen(welcome_message), 0);
    log_message("Paddock", "Connection established", "");
    
    while (1) {
        // Terima pesan dari driver.c
        valread = read(client_socket, buffer, 1024);
        if (valread > 0) {
            log_message("Driver", buffer, "");
            
            // Lakukan pemrosesan pesan dan berikan balasan
            char* command = strtok(buffer, " ");
            char* additional_info = strtok(NULL, " ");
            if (strcmp(command, "Gap") == 0) {
                float distance = atof(additional_info);
                strcpy(response, Gap(distance));
            } else if (strcmp(command, "Fuel") == 0) {
                float fuel_percentage = atof(additional_info);
                strcpy(response, Fuel(fuel_percentage));
            } else if (strcmp(command, "Tire") == 0) {
                int tire_usage = atoi(additional_info);
                strcpy(response, Tire(tire_usage));
            } else if (strcmp(command, "TireChange") == 0) {
                strcpy(response, TireChange(additional_info));
            } else {
                strcpy(response, "Invalid command");
            }
            
            // Kirim balasan ke driver.c
            send(client_socket, response, strlen(response), 0);
            log_message("Paddock", response, "");
        }
    }
}
