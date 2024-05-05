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
