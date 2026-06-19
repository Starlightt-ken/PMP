#include "login.hpp"
#include "user.hpp"             
#include "serial_utils.hpp"
#include "helper.hpp"
#include "inventory_system.hpp" 
#include "admin_menu.hpp"       
#include "user_general.hpp"     

User daftarUser[MAX_USERS];
int  totalUser       = 0;
int  indexUserAktif  = -1;

// MURNI VOID
static void toLowerCase(char* dst, const char* src) {
    int i = 0;
    while (src[i]) {
        dst[i] = (char)tolower((unsigned char)src[i]);
        i++;
    }
    dst[i] = '\0';
}

// MURNI VOID (Menggunakan pointer bool *hasil)
void cekUsernameAda(const char* username, bool *hasil) {
    char target[MAX_STR], current[MAX_STR];
    toLowerCase(target, username);
    *hasil = false; // Default: tidak ditemukan
    
    for (int i = 0; i < totalUser; i++) {
        toLowerCase(current, daftarUser[i].username);
        if (strcmp(current, target) == 0) {
            *hasil = true; // Ketemu!
            return;
        }
    }
}

// MURNI VOID
static void tampilkanPilihanRole(void) {
    Serial.println(F("\n  Pilih Role:"));
    Serial.println(F("  1. PIC"));
    Serial.println(F("  2. Admin"));
    Serial.println(F("  3. Peminjam"));
    Serial.print(F(" Pilih Role : "));
}

// MURNI VOID (Menggunakan pointer Role *output)
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

// MURNI VOID
void jalankanRegister(void) {
    Serial.println(F("\n >> DAFTAR ADMIN BARU"));

    if (totalUser >= MAX_USERS) {
        Serial.println(F("[!] Kapasitas memori akun penuh."));
        return;
    }

    char username[MAX_STR], password[MAX_STR];
    ErrorCode err = ERR_OK;
    bool isExist = false;

    Serial.print(F("  Username Admin Baru : "));
    readSerialString(username, &err);

    // Panggil fungsi VOID dengan pointer
    cekUsernameAda(username, &isExist);
    if (isExist) {
        Serial.println(F("Username sudah terdaftar!"));
        return;
    }

    Serial.print(F("  Password : "));
    readSerialString(password, &err);

    strcpy(daftarUser[totalUser].username, username);
    strcpy(daftarUser[totalUser].password, password);
    daftarUser[totalUser].role = ROLE_ADMIN;
    totalUser++;

    Serial.println(F("Akun Admin berhasil didaftarkan! Silakan login."));
}

// MURNI VOID
void jalankanLogin(InventoryList *l) {
    Serial.println(F("\n>>> LOGIN SYSTEM"));

    tampilkanPilihanRole();
    
    Role roleDipilih;
    bacaRole(&roleDipilih); // Panggil fungsi VOID dengan pointer
    
    if (roleDipilih == 0) {
        Serial.println(F("Pilihan role tidak valid."));
        return;
    }

    if (roleDipilih == ROLE_PEMINJAM) {
        Serial.println(F("\nBerhasil masuk sebagai Peminjam (Guest). Selamat datang!"));
        menuUserGeneral(l); 
        return; 
    }

    char username[MAX_STR], password[MAX_STR];
    ErrorCode err = ERR_OK;
    char buffer[10];

    if (roleDipilih == ROLE_ADMIN) {
        Serial.println(F("\n[MENU AKSES ADMIN]"));
        Serial.println(F("1. Masuk (Login Acc)"));
        Serial.println(F("2. Daftar Admin Baru (Register)"));
        Serial.print(F("Pilih tindakan (1-2): "));
        
        readSerialString(buffer, &err);
        if (buffer[0] == '2') {
            jalankanRegister();
            return; 
        }
    }

    Serial.println(F("\n--- MASUKKAN KREDENSIAL AKUN ---"));
    Serial.print(F("  Username : "));
    readSerialString(username, &err);
    
    Serial.print(F("  Password : "));
    readSerialString(password, &err);

    for (int i = 0; i < totalUser; i++) {
        if (strcasecmp(daftarUser[i].username, username) == 0) {
            if (daftarUser[i].role != roleDipilih) {
                Serial.println(F("Role tidak sesuai dengan akun terdaftar!"));
                return;
            }
            if (strcmp(daftarUser[i].password, password) != 0) {
                Serial.println(F("Password salah!"));
                return;
            }
    
            indexUserAktif = i; 
            Serial.print(F("\nLogin Berhasil! Selamat datang Admin/PIC: "));
            Serial.println(daftarUser[i].username);
            
            if (roleDipilih == ROLE_PIC) {
                process(l); 
            } else if (roleDipilih == ROLE_ADMIN) {
                menuAdmin(l); 
            }
            
            indexUserAktif = -1; 
            return;
        }
    }
    Serial.println(F("Username tidak ditemukan di database sistem."));
}

// MURNI VOID
void jalankanLogout(void) {
    Serial.println(F("\n>>> LOGOUT"));
    if (indexUserAktif == -1) {
        Serial.println(F("Logout gagal."));
        return;
    }
    Serial.println(F("Sampai jumpa!"));
    indexUserAktif = -1;  
}