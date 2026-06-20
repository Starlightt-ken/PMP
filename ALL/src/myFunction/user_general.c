#include "user_general.h"
#include "tampil_barang.h"
#include "pinjam_barang.h"
#include "pengembalian_barang.h"
#include "serial_utils.h"
#include "serial_bridge.h"
#include <stdbool.h>

void menuUserGeneral(InventoryList *l) {
    bool diMenuUser = true;
    ErrorCode err = ERR_OK;
    char buffer[MAX_LENGTH + 1];

    while (diMenuUser) {
        serial_cetak_teks_ln_flash(PSTR("\n==============================="));
        serial_cetak_teks_ln_flash(PSTR("     MENU PELANGGAN / USER     "));
        serial_cetak_teks_ln_flash(PSTR("==============================="));
        serial_cetak_teks_ln_flash(PSTR("1. Lihat Barang Tersedia"));
        serial_cetak_teks_ln_flash(PSTR("2. Pinjam Barang"));
        serial_cetak_teks_ln_flash(PSTR("3. Kembalikan Barang"));
        serial_cetak_teks_ln_flash(PSTR("4. Logout"));
        serial_cetak_teks_flash(PSTR("Pilih menu (1-4): "));

        readSerialString(buffer, &err);
        if (err != ERR_OK) continue;

        char choice = buffer[0];
        
        clearSerial(); 

        switch (choice) {
            case '1': 
                tampilkanBarangTersedia(l, &err); 
                break;
            case '2': 
                pinjamBarang(l, &err);          
                break;
            case '3': 
                kembalikanBarang(l, &err);       
                break;
            case '4': 
                serial_cetak_teks_ln_flash(PSTR("\nLogout berhasil. Kembali ke menu utama..."));
                diMenuUser = false; 
                break;
            default: 
                serial_cetak_teks_ln_flash(PSTR("\nPilihan tidak valid. Silakan coba lagi.")); 
                break;
        }
    }
}