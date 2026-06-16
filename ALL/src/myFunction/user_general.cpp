#include "user_general.hpp"

// DISINI KITA PANGGIL FILE-FILE TERPISAH TADI!
#include "tampil_barang.hpp"
#include "pinjam_barang.hpp"
#include "pengembalian_barang.hpp"
#include "serial_utils.hpp"

void menuUserGeneral(InventoryList *l) {
    bool diMenuUser = true;
    ErrorCode err = ERR_OK;
    char buffer[MAX_LENGTH + 1];

    while (diMenuUser) {
        Serial.println(F("\n==============================="));
        Serial.println(F("     MENU PELANGGAN / USER     "));
        Serial.println(F("==============================="));
        Serial.println(F("1. Lihat Barang Tersedia"));
        Serial.println(F("2. Pinjam Barang"));
        Serial.println(F("3. Kembalikan Barang"));
        Serial.println(F("4. Logout"));
        Serial.print(F("Pilih menu (1-4): "));

        readSerialString(buffer, &err);
        if (err != ERR_OK) continue;

        char choice = buffer[0];
        clearSerial();

        switch (choice) {
            case '1': 
                tampilkanBarangTersedia(l, &err); // Diambil dari tampil_barang.cpp
                break;
            case '2': 
                pinjamBarang(l, &err);           // Diambil dari pinjam_barang.cpp
                break;
            case '3': 
                kembalikanBarang(l, &err);       // Diambil dari pengembalian_barang.cpp
                break;
            case '4': 
                Serial.println(F("\nLogout berhasil. Kembali ke menu utama..."));
                diMenuUser = false; 
                break;
            default: 
                Serial.println(F("\nPilihan tidak valid. Silakan coba lagi.")); 
                break;
        }
    }
}