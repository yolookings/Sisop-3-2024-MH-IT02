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

// Fungsi untuk menentukan aksi berdasarkan jarak dengan musuh di depan
char* Gap(float distance) {
    if (distance < 3.5) {
        return "Gogogo";
    } else if (distance >= 3.5 && distance <= 10) {
        return "Push";
    } else {
        return "Stay out of trouble";
    }
}

// Fungsi untuk menentukan aksi berdasarkan sisa bensin
char* Fuel(float fuel_percentage) {
    if (fuel_percentage > 80) {
        return "Push Push Push";
    } else if (fuel_percentage >= 50 && fuel_percentage <= 80) {
        return "You can go";
    } else {
        return "Conserve Fuel";
    }
}

// Fungsi untuk menentukan aksi berdasarkan sisa pemakaian ban
char* Tire(int tire_usage) {
    if (tire_usage > 80) {
        return "Go Push Go Push";
    } else if (tire_usage >= 50 && tire_usage <= 80) {
        return "Good Tire Wear";
    } else if (tire_usage >= 30 && tire_usage < 50) {
        return "Conserve Your Tire";
    } else {
        return "Box Box Box";
    }
}

// Fungsi untuk menentukan aksi berdasarkan tipe ban saat ini
char* TireChange(char* current_tire_type) {
    if (strcmp(current_tire_type, "Soft") == 0) {
        return "Mediums Ready";
    } else if (strcmp(current_tire_type, "Medium") == 0) {
        return "Box for Softs";
    } else {
        return "Invalid tire type";
    }
}

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
        fprintf(log_file, "[%s] [%s]: [%s] [%s]\n", source, time_str, command, additional_info);
        fclose(log_file);
    }
}

// Fungsi untuk menangani koneksi dari driver.c
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

int main() {
    int server_fd, client_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    
    // Membuat socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    
    // Menetapkan opsi socket
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    // Binding socket ke alamat dan port tertentu
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    
    // Menandai socket untuk mendengarkan koneksi
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    
    // Menerima koneksi dari driver.c
    while (1) {
        if ((client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        // Handle koneksi dari driver.c
        handle_client(client_socket);
        close(client_socket);
    }
    return 0;
}
