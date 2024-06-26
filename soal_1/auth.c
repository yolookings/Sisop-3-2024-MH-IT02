#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHMSZ 1024

void createDirectory(const char *path) {
    if (mkdir(path, 0777) == -1) {
        perror("mkdir");
        exit(EXIT_FAILURE);
    }
}

void createFile(const char *path) {
    FILE *file = fopen(path, "w");
    if (file == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    fclose(file);
}

int main() {
    // Membuat direktori new-data jika belum ada
    createDirectory("new-data");
    createFile("new-data/belobog_trashcan.csv");
    createFile("new-data/ikn.csv");
    createFile("new-data/osaka_parkinglot.csv");

    // Membuat direktori microservices/database jika belum ada
    createDirectory("microservices");
    createDirectory("microservices/database");

    // Membuat file db.log jika belum ada
    createFile("microservices/database/db.log");
    
    // Memindahkan file db.c ke dalam folder microservices
    if (rename("db.c", "microservices/db.c") == -1) {
        perror("rename");
        exit(EXIT_FAILURE);
    }
    
    // Memindahkan file rate.c ke dalam folder microservices
    if (rename("rate.c", "microservices/rate.c") == -1) {
        perror("rename");
        exit(EXIT_FAILURE);
    }

    DIR *dir;
    struct dirent *ent;
    struct stat st;
    key_t key;
    int shmid;
    char *shm;

    // Mendapatkan kunci untuk shared memory
    if ((key = ftok("auth.c", 'R')) == -1) {
        perror("ftok");
        exit(1);
    }

    // Membuat shared memory
    if ((shmid = shmget(key, SHMSZ, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    }

    // Menyambungkan ke shared memory
    if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat");
        exit(1);
    }

    // Membuka folder new-data
    dir = opendir("new-data");
    if (dir != NULL) {
        // Loop untuk membaca setiap file dalam folder
        while ((ent = readdir(dir)) != NULL) {
            char filename[256];
            strcpy(filename, "new-data/");
            strcat(filename, ent->d_name);

            // Memeriksa apakah file adalah file regular
            if (stat(filename, &st) == 0 && S_ISREG(st.st_mode)) {
                // Memeriksa apakah file berakhiran dengan "trashcan" atau "parkinglot" dan berekstensi ".csv"
                if ((strstr(ent->d_name, "_trashcan.csv") != NULL || strstr(ent->d_name, "_parkinglot.csv") != NULL)) {
                    // Mengkopi isi file ke shared memory
                    FILE *file = fopen(filename, "r");
                    if (file != NULL) {
                        fread(shm, sizeof(char), SHMSZ, file);
                        fclose(file);
                    }
                } else {
                    // Jika tidak memenuhi syarat, hapus file tersebut
                    remove(filename);
                }
            }
        }
        closedir(dir);
    } else {
        perror("opendir");
        exit(1);
    }

    // Menutup shared memory
    shmdt(shm);

    return 0;
}
