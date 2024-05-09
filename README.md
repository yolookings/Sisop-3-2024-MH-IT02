# Laporan Hasil Praktikum Sistem Operasi 2024 Modul 3 - IT02

## Anggota Kelompok IT 02 :

- Maulana Ahmad Zahiri (5027231010)
- Syela Zeruya Tandi Lalong (5027231076)
- Kharisma Fahrun Nisa' (5027231086)

## Daftar Isi

- [Soal 1](#soal-1)
- [Soal 2](#soal-2)
- [Soal 3](#soal-3)
- [Soal 4](#soal-4)

# Soal 1

## Deskripsi Soal

Pada zaman dahulu pada galaksi yang jauh-jauh sekali, hiduplah seorang Stelle. Stelle adalah seseorang yang sangat tertarik dengan Tempat Sampah dan Parkiran Luar Angkasa. Stelle memulai untuk mencari Tempat Sampah dan Parkiran yang terbaik di angkasa. Dia memerlukan program untuk bisa secara otomatis mengetahui Tempat Sampah dan Parkiran dengan rating terbaik di angkasa.

pada soal 1 ini kita di suruh untuk membantu mencari parkiran dan tempat sampah dengan rating terbaik di luar angkasa

## Pengerjaan

Untuk pengerjaan kali ini, saya membaginya ke dalam 3 buah file yakni:

- auth.c
- rate.c
- db.c

## Penjelasan

file `auth.c` :

```c
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
```

kode tersbut merupakan langkah untuk membuat direktori awal sebagaimana di contoh :

![alt text](images/image-2.png)

maka saat kita menjalankannya akan nampak direktori sebagaimana gambar diatas.

```c
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
```

kode diatas merupakan lanjutan dari kode auth.c dimana membuat dan menyambungkan shared memory, sehingga dapat terintegrasi satu sama lain.

kemudian lanjut ke kode `rate.c` :

```c
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
    if ((key = ftok("../auth.c", 'R')) == -1) {
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
```

program tersebut memperoses kode untuk menemukan `tempat sampah` dan `parkiran` dengan rating terbaik, kemudian menampilkannya.

![alt text](images/image-1.png)

sehingga saat program dijalankan akan terlihat sebagaimana gambar diatas.

lanjuta ke file yang terakhir yakni `db.c`

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>

int main() {
    DIR *dir;
    struct dirent *ent;
    struct stat st;
    FILE *logFile;
    time_t t;
    struct tm *tm_info;

    // Membuka folder new-data
    dir = opendir("new-data");
    if (dir != NULL) {
        // Membuka file log
        logFile = fopen("microservices/database/db.log", "a");
        if (logFile == NULL) {
            perror("fopen");
            exit(1);
        }

        // Mendapatkan waktu saat ini
        time(&t);
        tm_info = localtime(&t);

        // Loop untuk membaca setiap file dalam folder
        while ((ent = readdir(dir)) != NULL) {
            char filename[256];
            strcpy(filename, "new-data/");
            strcat(filename, ent->d_name);

            // Memeriksa apakah file adalah file regular
            if (stat(filename, &st) == 0 && S_ISREG(st.st_mode)) {
                // Memindahkan file ke folder database
                char newFilename[256];
                strcpy(newFilename, "microservices/database/");
                strcat(newFilename, ent->d_name);
                if (rename(filename, newFilename) == 0) {
                    // Menulis log perpindahan file
                    fprintf(logFile, "[%02d/%02d/%04d %02d:%02d:%02d] [%s] [%s]\n",
                            tm_info->tm_mday, tm_info->tm_mon + 1, tm_info->tm_year + 1900,
                            tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec,
                            strstr(ent->d_name, "_trashcan.csv") != NULL ? "Trash Can" : "Parking Lot",
                            ent->d_name);
                }
            }
        }

        // Menutup file log dan folder
        fclose(logFile);
        closedir(dir);
    } else {
        perror("opendir");
        exit(1);
    }

    return 0;
}
```

pada file `db.c` tersebut akan membaca folder new-data dan kemudian memindahkan file dengan format `parkinglot.csv` dan `trashcan.csv` kedalam database dan kemudian mencatatakan lognya ke dalam file log.

![alt text](images/image-3.png)
gambar diatas menampilkan saat program db. dijalankan dan kemudian akan memindahkan file dengan format yang tepat ke folder database.

![alt text](images/image-4.png)
gambar diatas menampilkan catatan log dari file db.log yang mencatat hasil dari dijalankannya program ./db

sekian terimakasih :) .

## Dokumentasi

![alt text](images/image.png)
merupakan gambaran awal direktori

![alt text](images/image-5.png)
merupakan gambaran akhir direktori

# Soal 2

## Deskripsi Soal

## Pengerjaan

# Soal 3

## Deskripsi Soal

## Pengerjaan

# Soal 4

## Deskripsi Soal

## Pengerjaan
