#include "login.h"
#include "user.h"             
#include "serial_utils.h"
#include "helper.h"
#include "inventory_system.h" 
#include "admin_menu.h"       
#include "user_general.h"     
#include "serial_bridge.h" 
#include <string.h>
#include <ctype.h>

User daftarUser[MAX_USERS];
int  totalUser       = 0;
int  indexUserAktif  = -1;

static void toLowerCase(char* dst, const char* src) {
    int i = 0;
    while (src[i]) {
        dst[i] = (char)tolower((unsigned char)src[i]);
        i++;
    }
    dst[i] = '\0';
}

void cekUsernameAda(const char* username, bool *hasil) {
    char target[MAX_STR], current[MAX_STR];
    toLowerCase(target, username);
    *hasil = false; 
    
    for (int i = 0; i < totalUser; i++) {
        toLowerCase(current, daftarUser[i].username);
        if (strcmp(current, target) == 0) {
            *hasil = true; 
            return;
        }
    }
}

static void tampilkanPilihanRole(void) {
    serial_cetak_teks_ln_flash(PSTR("\n  Pilih Role:"));
    serial_cetak_teks_ln_flash(PSTR("  1. PIC"));
    serial_cetak_teks_ln_flash(PSTR("  2. Admin"));
    serial_cetak_teks_ln_flash(PSTR("  3. Peminjam"));
    serial_cetak_teks_flash(PSTR(" Pilih Role : "));
}

static void bacaRole(Role *output) {
    ErrorCode err = ERR_OK;
    char buffer[10];
    readSerialString(buffer, &err);
    
    uint8_t pilihan = 0;
    stringToInt(buffer, &pilihan, &err);
    
    if (pilihan < 1 || pilihan > 3) {
        *output = (Role)0;
    } else {
        *output = (Role)pilihan;
    }
}

void jalankanRegister(void) {
    serial_cetak_teks_ln_flash(PSTR("\n >> DAFTAR ADMIN BARU"));

    if (totalUser >= MAX_USERS) {
        serial_cetak_teks_ln_flash(PSTR("[!] Kapasitas memori akun penuh."));
        return;
    }

    char username[MAX_STR], password[MAX_STR];
    ErrorCode err = ERR_OK;
    bool isExist = false;

    serial_cetak_teks_flash(PSTR("  Username Admin Baru : "));
    readSerialString(username, &err);

    cekUsernameAda(username, &isExist);
    if (isExist) {
        serial_cetak_teks_ln_flash(PSTR("Username sudah terdaftar!"));
        return;
    }

    serial_cetak_teks_flash(PSTR("  Password : "));
    readSerialString(password, &err);

    strcpy(daftarUser[totalUser].username, username);
    strcpy(daftarUser[totalUser].password, password);
    daftarUser[totalUser].role = ROLE_ADMIN;
    totalUser++;

    serial_cetak_teks_ln_flash(PSTR("Akun Admin berhasil didaftarkan! Silakan login."));
}

void jalankanLogin(InventoryList *l) {
    serial_cetak_teks_ln_flash(PSTR("\n>>> LOGIN SYSTEM"));

    tampilkanPilihanRole();
    
    Role roleDipilih;
    bacaRole(&roleDipilih); 
    
    if (roleDipilih == 0) {
        serial_cetak_teks_ln_flash(PSTR("Pilihan role tidak valid."));
        return;
    }

    if (roleDipilih == ROLE_PEMINJAM) {
        serial_cetak_teks_ln_flash(PSTR("\nBerhasil masuk sebagai Peminjam (Guest). Selamat datang!"));
        menuUserGeneral(l); 
        return; 
    }

    char username[MAX_STR], password[MAX_STR];
    ErrorCode err = ERR_OK;
    char buffer[10];

    if (roleDipilih == ROLE_ADMIN) {
        serial_cetak_teks_ln_flash(PSTR("\n[MENU AKSES ADMIN]"));
        serial_cetak_teks_ln_flash(PSTR("1. Masuk (Login Acc)"));
        serial_cetak_teks_ln_flash(PSTR("2. Daftar Admin Baru (Register)"));
        serial_cetak_teks_flash(PSTR("Pilih tindakan (1-2): "));
        
        readSerialString(buffer, &err);
        uint8_t choice = 0;
        stringToInt(buffer, &choice, &err);
        if (err != ERR_OK) {
            serial_cetak_teks_ln_flash(PSTR("Pilihan yang dimasukan salah"));
            return;
        }
        if (buffer[0] == '2') {
            jalankanRegister();
            return;
        }
    }

    serial_cetak_teks_ln_flash(PSTR("\n--- MASUKKAN KREDENSIAL AKUN ---"));
    serial_cetak_teks_flash(PSTR("  Username : "));
    readSerialString(username, &err);
    
    serial_cetak_teks_flash(PSTR("  Password : "));
    readSerialString(password, &err);

    for (int i = 0; i < totalUser; i++) {
        char target_user[MAX_STR], current_user[MAX_STR];
        toLowerCase(target_user, username);
        toLowerCase(current_user, daftarUser[i].username);

        if (strcmp(current_user, target_user) == 0) {
            if (daftarUser[i].role != roleDipilih) {
                serial_cetak_teks_ln_flash(PSTR("Role tidak sesuai dengan akun terdaftar!"));
                return;
            }
            if (strcmp(daftarUser[i].password, password) != 0) {
                serial_cetak_teks_ln_flash(PSTR("Password salah!"));
                return;
            }
    
            indexUserAktif = i; 
            serial_cetak_teks_flash(PSTR("\nLogin Berhasil! Selamat datang Admin/PIC: "));
            serial_cetak_teks_ln(daftarUser[i].username);
            
            if (roleDipilih == ROLE_PIC) {
                process(l); 
            } else if (roleDipilih == ROLE_ADMIN) {
                menuAdmin(l); 
            }
            
            indexUserAktif = -1; 
            return;
        }
    }
    serial_cetak_teks_ln_flash(PSTR("Username tidak ditemukan di database sistem."));
}

void jalankanLogout(void) {
    serial_cetak_teks_ln_flash(PSTR("\n>>> LOGOUT"));
    if (indexUserAktif == -1) {
        serial_cetak_teks_ln_flash(PSTR("Logout gagal."));
        return;
    }
    serial_cetak_teks_ln_flash(PSTR("Sampai jumpa!"));
    indexUserAktif = -1;  
}
