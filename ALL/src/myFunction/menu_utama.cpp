#include "menu_utama.hpp"
#include "login.hpp"
#include "serial_utils.hpp"

void tampilkanMenuUtama(void) {
    Serial.println(F("\n         Menu Utama                    "));
    Serial.println(F("1. Login                              "));
    Serial.println(F("2. Register (Daftar Akun Baru)        "));
    Serial.println(F("3. Lupa Password                      "));
    Serial.println(F("4. Keluar                             "));
    Serial.print(F("Pilih menu : "));
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
            case '1': jalankanLogin(l); break;
            case '2': jalankanRegister(); break;
            case '3': jalankanLupaPassword(); break;
            case '4': 
                Serial.println(F("\nProgram dihentikan.")); 
                return; 
            default: 
                Serial.println(F("[!] Pilihan tidak valid.")); 
                break;
        }
    }
}