#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/wait.h>

// Deklarasi fungsi konversi angka ke kalimat
char *konversiAngkaKeKalimat(int angka);

// Deklarasi fungsi untuk mengubah string menjadi angka
int konversiStringKeAngka(char *str);

// Deklarasi fungsi kalkulasi
int kalkulasi(int angka1, int angka2, char *operator);

int main(int argc, char *argv[]) {
  // Memeriksa argumen program
  if (argc != 4) {
    printf("Penggunaan: %s -operator <angka1> <angka2>\n", argv[0]);
    printf("Contoh: %s -tambah tiga tujuh\n", argv[0]);
    exit(1);
  }

  // Mendapatkan operator dari argumen program
  char *operator = argv[1];

  // Mendapatkan angka pertama dan kedua dari argumen program
  int angka1 = konversiStringKeAngka(argv[2]);
  int angka2 = konversiStringKeAngka(argv[3]);

  // Membuat pipes
  int fd1[2], fd2[2];
  if (pipe(fd1) == -1 || pipe(fd2) == -1) {
    perror("Error membuat pipe");
    exit(1);
  }

  // Forking proses
  pid_t pid;
  pid = fork();
  if (pid == -1) {
    perror("Error forking");
    exit(1);
  }

  // Parent process
  if (pid > 0) {
    // Menutup deskriptor pipa yang tidak digunakan
    close(fd1[0]);
    close(fd2[1]);

    // Mengirim angka ke child process melalui pipe
    write(fd1[1], &angka1, sizeof(angka1));
    write(fd1[1], &angka2, sizeof(angka2));

    // Menerima hasil kalkulasi dari child process
    int hasil;
    read(fd2[0], &hasil, sizeof(hasil));

    // Mengonversi hasil menjadi kalimat
    char *kalimat = konversiAngkaKeKalimat(hasil);

    // Get current time
    time_t now = time(NULL);
    struct tm *tm = localtime(&now);

    // Format message
    char operatorStr[20], operator1[20];
    if (strcmp(operator, "-kali") == 0) {
      strcpy(operatorStr, "KALI");
      strcpy(operator1, "kali");
    }
    else if (strcmp(operator, "-tambah") == 0) {
      strcpy(operatorStr, "TAMBAH");
      strcpy(operator1, "tambah");
    }
    else if (strcmp(operator, "-kurang") == 0) {
      strcpy(operatorStr, "KURANG");
      strcpy(operator1, "kurang");
    }
    else if (strcmp(operator, "-bagi") == 0) {
      strcpy(operatorStr, "BAGI");
      strcpy(operator1, "bagi");
    }
    else {
      strcpy(operatorStr, "OPERASI");
    }

    char message[1024];
    sprintf(message, "[%02d/%02d/%02d %02d:%02d:%02d] [%s] %s %s %s sama dengan %s.\n",
            tm->tm_mday, tm->tm_mon + 1, tm->tm_year % 100, tm->tm_hour, tm->tm_min, tm->tm_sec,
            operatorStr, argv[2], operator1, argv[3], kalimat);

    // Menulis message log ke file
    int logFile = open("histori.log", O_WRONLY | O_APPEND | O_CREAT, 0644);
    if (logFile == -1) {
      perror("Error membuka/membuat file log");
      exit(1);
    }
    write(logFile, message, strlen(message));
    close(logFile);

    // Menunggu child process selesai
    wait(NULL);
  }
  // Child process
  else {
    // Menutup deskriptor pipa yang tidak digunakan
    close(fd1[1]);
    close(fd2[0]);

    // Menerima angka dari parent process
    int angka1, angka2;
    read(fd1[0], &angka1, sizeof(angka1));
    read(fd1[0], &angka2, sizeof(angka2));

    // Melakukan kalkulasi
    int hasil;
    if (strcmp(operator, "-kali") == 0)
      hasil = angka1 * angka2;
    else if (strcmp(operator, "-tambah") == 0)
      hasil = angka1 + angka2;
    else if (strcmp(operator, "-kurang") == 0)
      hasil = angka1 - angka2;
    else if (strcmp(operator, "-bagi") == 0) {
      if (angka2 == 0) {
        printf("Error: Pembagian dengan nol tidak diizinkan.\n");
        exit(1);
      }
      hasil = angka1 / angka2;
    }
    else {
      printf("Error: Operator tidak valid.\n");
      exit(1);
    }

    // Mengirim hasil kalkulasi ke parent process melalui pipe
    write(fd2[1], &hasil, sizeof(hasil));
  }

  return 0;
}

// Implementasi fungsi untuk mengubah string menjadi angka
int konversiStringKeAngka(char *str) {
  if (strcmp(str, "nol") == 0) return 0;
  else if (strcmp(str, "satu") == 0) return 1;
  else if (strcmp(str, "dua") == 0) return 2;
  else if (strcmp(str, "tiga") == 0) return 3;
  else if (strcmp(str, "empat") == 0) return 4;
  else if (strcmp(str, "lima") == 0) return 5;
  else if (strcmp(str, "enam") == 0) return 6;
  else if (strcmp(str, "tujuh") == 0) return 7;
  else if (strcmp(str, "delapan") == 0) return 8;
  else if (strcmp(str, "sembilan") == 0) return 9;
  else if (strcmp(str, "sepuluh") == 0) return 10;
  else if (strcmp(str, "sebelas") == 0) return 11;
  else if (strcmp(str, "dua belas") == 0) return 12;
  else if (strcmp(str, "tiga belas") == 0) return 13;
  else if (strcmp(str, "empat belas") == 0) return 14;
  else if (strcmp(str, "lima belas") == 0) return 15;
  else if (strcmp(str, "enam belas") == 0) return 16;
  else if (strcmp(str, "tujuh belas") == 0) return 17;
  else if (strcmp(str, "delapan belas") == 0) return 18;
  else if (strcmp(str, "sembilan belas") == 0) return 19;
  else if (strcmp(str, "dua puluh") == 0) return 20;
  else if (strcmp(str, "tiga puluh") == 0) return 30;
  else if (strcmp(str, "empat puluh") == 0) return 40;
  else if (strcmp(str, "lima puluh") == 0) return 50;
  else if (strcmp(str, "enam puluh") == 0) return 60;
  else if (strcmp(str, "tujuh puluh") == 0) return 70;
  else if (strcmp(str, "delapan puluh") == 0) return 80;
  else if (strcmp(str, "sembilan puluh") == 0) return 90;
  else {
    printf("ERROR: Input tidak valid.\n");
    exit(1);
  }
}

// Implementasi fungsi kalkulasi
int kalkulasi(int angka1, int angka2, char *operator) {
  if (strcmp(operator, "-kali") == 0)
    return angka1 * angka2;
  else if (strcmp(operator, "-tambah") == 0)
    return angka1 + angka2;
  else if (strcmp(operator, "-kurang") == 0)
    return angka1 - angka2;
  else if (strcmp(operator, "-bagi") == 0) {
    if (angka2 == 0) {
      printf("ERROR: Pembagian dengan nol tidak diizinkan.\n");
      exit(1);
    }
    return angka1 / angka2;
  } else {
    printf("ERROR: Operator tidak valid.\n");
    exit(1);
  }
}

// Implementasi fungsi untuk konversi angka menjadi kalimat
char *konversiAngkaKeKalimat(int angka) {
  char *kalimat = (char *)malloc(100 * sizeof(char)); // alokasi memori awal untuk string hasil
  if (kalimat == NULL) {
    printf("ERROR: Gagal mengalokasi memori.\n");
    exit(1);
  }

  switch (angka) {
    // kasus-kasus sebelum 20
    case 0:
      strcpy(kalimat, "nol");
      break;
    case 1:
      strcpy(kalimat, "satu");
      break;
    case 2:
      kalimat = "dua";
      break;
    case 3:
      kalimat = "tiga";
      break;
    case 4:
      kalimat = "empat";
      break;
    case 5:
      kalimat = "lima";
      break;
    case 6:
      kalimat = "enam";
      break;
    case 7:
      kalimat = "tujuh";
      break;
    case 8:
      kalimat = "delapan";
      break;
    case 9:
      kalimat = "sembilan";
      break;
    case 10:
      kalimat = "sepuluh";
      break;
    case 11:
      kalimat = "sebelas";
      break;
    case 12:
      kalimat = "dua belas";
      break;
    case 13:
      kalimat = "tiga belas";
      break;
    case 14:
      kalimat = "empat belas";
      break;
    case 15:
      kalimat = "lima belas";
      break;
    case 16:
      kalimat = "enam belas";
      break;
    case 17:
      kalimat = "tujuh belas";
      break;
    case 18:
      kalimat = "delapan belas";
      break;
    case 19:
      kalimat = "sembilan belas";
      break;
    // kasus-kasus lainnya
    case 20:
      strcpy(kalimat, "dua puluh");
      break;
    case 30:
      strcpy(kalimat, "tiga puluh");
      break;
    case 40:
      strcpy(kalimat, "empat puluh");
      break;
    case 50:
      strcpy(kalimat, "lima puluh");
      break;
    case 60:
      strcpy(kalimat, "enam puluh");
      break;
    case 70:
      strcpy(kalimat, "tujuh puluh");
      break;
    case 80:
      strcpy(kalimat, "delapan puluh");
      break;
    case 90:
      strcpy(kalimat, "sembilan puluh");
      break;
    default:
      if (angka > 20 && angka < 100) {
        strcpy(kalimat, konversiAngkaKeKalimat((angka / 10) * 10)); // puluhan
        strcat(kalimat, " ");
        strcat(kalimat, konversiAngkaKeKalimat(angka % 10)); // satuan
      } else {
        strcpy(kalimat, "tidak diketahui");
      }
  }
  return kalimat;
}
