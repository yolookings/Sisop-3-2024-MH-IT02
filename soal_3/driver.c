#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <time.h>

#define PORT 8080
#define LOG_FILE "race.log"

// Deklarasi fungsi log_message
void log_message(char* source, char* command, char* additional_info);

int main(int argc, char *argv[]) {
    if (argc != 5 || strcmp(argv[1], "-c") != 0) {
        printf("Usage: %s -c [Command] -i [Info]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    char* command = argv[2];
    char* info = argv[4];
    
    // Log pesan ke file
        log_message("Driver", command, info);

    // Koneksi ke server paddock
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    
    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
    
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }
    
    // Kirim pesan ke server paddock
    char message[100];
    sprintf(message, "%s %s", command, info);
    send(sock , message , strlen(message) , 0 );
    
    // Hanya log pesan ke file jika pesan bukan merupakan balasan dari server
    if (strcmp(command, "Fuel") != 0 || strcmp(info, "55%") != 0) {
        // Terima balasan dari server paddock
        read(sock , buffer, 1024);
        printf("%s\n", buffer);
    }
    
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
    } else {
        perror("log_message: fopen");
        exit(EXIT_FAILURE);
    }
}
