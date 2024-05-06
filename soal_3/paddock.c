#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <time.h>
#include "actions.c"

#define PORT 8080
#define LOG_FILE "race.log"

// Fungsi untuk melakukan logging ke file race.log
void log_message(char* source, char* command, char* additional_info) {
    time_t current_time;
    struct tm* time_info;
    char time_str[20];
    
    time(&current_time);
    time_info = localtime(&current_time);
    strftime(time_str, sizeof(time_str), "%d/%m/%Y %H:%M:%S", time_info);
    
    FILE* log_file = fopen(LOG_FILE, "a");
    if (log_file != NULL) {
        if (additional_info && strlen(additional_info) > 0) {
            fprintf(log_file, "[%s] [%s]: [%s] [%s]\n", source, time_str, command, additional_info);
        } else {
            fprintf(log_file, "[%s] [%s]: [%s]\n", source, time_str, command);
        }
        fclose(log_file);
    } else {
        perror("log_message: fopen");
        exit(EXIT_FAILURE);
    }
}


// Fungsi untuk menangani koneksi dari driver.c
void handle_client(int client_socket) {
    char buffer[1024] = {0};
    int valread;
    char response[1024] = {0};
    
    // Kirim pesan selamat datang ke driver.c hanya sekali saat koneksi pertama kali dibuat
    static int connection_established = 0;
    if (!connection_established) {
        char* welcome_message = "Welcome to Paddock!";
        send(client_socket, welcome_message, strlen(welcome_message), 0);
        log_message("Paddock", "Connection established", "");
        connection_established = 1;
    }
    
    while (1) {
        // Terima pesan dari driver.c
        valread = read(client_socket, buffer, 1024);
        if (valread > 0) {
            // Lakukan pemrosesan pesan dan berikan balasan
            char* command = strtok(buffer, " ");
            char* additional_info = strtok(NULL, "\0"); // Mengubah delimiter menjadi NULL untuk mendapatkan keseluruhan additional_info
            if (command != NULL && additional_info != NULL) {
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
            } else {
                strcpy(response, "Error: Invalid message format");
            }
            
            // Kirim balasan ke driver.c
            if (send(client_socket, response, strlen(response), 0) < 0) {
                perror("handle_client: send");
                log_message("Paddock", "Error sending response", "");
            } else {
                log_message("Driver", buffer, "");
                log_message("Paddock", response, "");
            }
        } else if (valread == 0) {
            // Koneksi ditutup oleh driver.c
            break;
        } else {
            perror("handle_client: read");
            log_message("Paddock", "Error reading from client", "");
            break;
        }
    }
}


int main() {
    int server_fd, client_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    
    // Membuat socket file descriptor
    int sock;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        return -1;
    }
    
    // Menetapkan opsi socket
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    // Binding socket ke alamat dan port tertentu
    if (bind(sock, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    
    // Menandai socket untuk mendengarkan koneksi
    if (listen(sock, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    
    // Menerima koneksi dari driver.c
    while (1) {
        if ((client_socket = accept(sock, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        // Handle koneksi dari driver.c
        handle_client(client_socket);
        close(client_socket);
    }
    return 0;
}
