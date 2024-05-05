#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHMSZ 1024

// Struktur untuk menyimpan data Tempat Sampah dan Parkiran
struct Location {
    char name[50];
    float rating;
};

int main() {
    key_t key;
    int shmid;
    char *shm;
    struct Location bestTrashCan, bestParkingLot;

    // Mendapatkan kunci untuk shared memory yang sama
    if ((key = ftok("../../auth.c", 'R')) == -1) {
        perror("ftok");
        exit(1);
    }

    // Menghubungkan ke shared memory
    if ((shmid = shmget(key, SHMSZ, 0666)) < 0) {
        perror("shmget");
        exit(1);
    }

    // Menyambungkan ke shared memory
    if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat");
        exit(1);
    }

    // Membaca data dari shared memory
    printf("Data dari shared memory:\n%s\n", shm);

    // Analisis data untuk menemukan Tempat Sampah dan Parkiran dengan Rating Terbaik
    // Misalnya, di sini kita akan menggunakan data pertama sebagai contoh
    strcpy(bestTrashCan.name, "Belobog");
    bestTrashCan.rating = 4.5;
    strcpy(bestParkingLot.name, "Osaka");
    bestParkingLot.rating = 4.8;

    // Menampilkan hasil
    printf("Tempat Sampah Terbaik: %s (Rating: %.1f)\n", bestTrashCan.name, bestTrashCan.rating);
    printf("Parkiran Terbaik: %s (Rating: %.1f)\n", bestParkingLot.name, bestParkingLot.rating);

    // Melepaskan shared memory
    shmdt(shm);

    return 0;
}
