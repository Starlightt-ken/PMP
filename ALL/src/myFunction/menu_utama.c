#include "menu_utama.h"
#include "login.h"
#include "serial_utils.h"
#include "serial_bridge.h" 
#include <stdbool.h>

void tampilkanMenuUtama(void) {
    serial_cetak_teks_ln_flash(PSTR("\n        Menu Utama                    "));
    serial_cetak_teks_ln_flash(PSTR("1. Login                              "));
    serial_cetak_teks_ln_flash(PSTR("2. Keluar                             "));
    serial_cetak_teks_flash(PSTR("Pilih menu (1-2): "));
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
                serial_cetak_teks_ln_flash(PSTR("\nProgram dihentikan.")); 
                return; 
            default: 
                serial_cetak_teks_ln_flash(PSTR("[!] Pilihan tidak valid.")); 
                break;
        }
    }
}