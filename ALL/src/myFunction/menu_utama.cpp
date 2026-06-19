#include "menu_utama.hpp"
#include "login.hpp"
#include "serial_utils.hpp"

void tampilkanMenuUtama(void) {
    Serial.println(F("\n        Menu Utama                    "));
    Serial.println(F("1. Login                              "));
    Serial.println(F("2. Keluar                             "));
    Serial.print(F("Pilih menu (1-2): "));
}

void jalankanMenuUtama(InventoryList *l) {
    char buffer[10];
    ErrorCode err = ERR_OK;

    while (true) {
        tampilkanMenuUtama();

        readSerialString(buffer, &err);
        if (err != ERR_OK) continue;

        char pilihan = buffer[0];

        switch (pilihan) {
            case '1': 
                jalankanLogin(l); 
                break;
            case '2': 
                Serial.println(F("\nProgram dihentikan.")); 
                return; 
            default: 
                Serial.println(F("[!] Pilihan tidak valid.")); 
                break;
        }
    }
}