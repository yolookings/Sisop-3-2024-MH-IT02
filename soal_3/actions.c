#include <stdio.h>
#include <string.h>

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
