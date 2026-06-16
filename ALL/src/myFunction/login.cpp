#include "login.hpp"
#include "user.hpp"             // WAJIB ADA: Memanggil definisi struct user
#include "serial_utils.hpp"
#include "helper.hpp"
#include "inventory_system.hpp" // Memanggil menu PIC
#include "admin_menu.hpp"       // Memanggil menu Admin 
#include "user_general.hpp"     // Memanggil menu Peminjam (Buatanmu)

// Inisialisasi variabel global dari user.hpp
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

int usernameAda(const char* username) {
    char target[MAX_STR], current[MAX_STR];
    toLowerCase(target, username);
    for (int i = 0; i < totalUser; i++) {
        toLowerCase(current, daftarUser[i].username);
        if (strcmp(current, target) == 0) return 1;
    }
    return 0;
}

static void tampilkanPilihanRole(void) {
    Serial.println(F("\n  Pilih Role:"));
    Serial.println(F("  1. PIC"));
    Serial.println(F("  2. Admin"));
    Serial.println(F("  3. Peminjam"));
    Serial.print(F(" Pilih Role : "));
}

static Role bacaRole(void) {
    ErrorCode err = ERR_OK;
    char buffer[10];
    readSerialString(buffer, &err);
    
    uint8_t pilihan = 0;
    stringToInt(buffer, &pilihan, &err);
    
    if (pilihan < 1 || pilihan > 3) return (Role)0;
    return (Role)pilihan;
}

static void runLengthEncode(char* dst, const char* src) {
    int len = (int)strlen(src);
    int count = 1, j = 0;
    for (int i = 1; i <= len; i++) {
        if (i < len && src[i] == src[i - 1]) {
            count++;
        } else {
            if (count > 1) dst[j++] = (char)(count + '0');
            dst[j++] = src[i - 1];
            count = 1;
        }
    }
    dst[j] = '\0';
}

void jalankanLogin(InventoryList *l) {
    Serial.println(F("\n>>> LOGIN"));

    tampilkanPilihanRole();
    Role roleDipilih = bacaRole();
    if (roleDipilih == 0) {
        Serial.println(F("Pilihan role tidak valid."));
        return;
    }

    char username[MAX_STR], password[MAX_STR];
    ErrorCode err = ERR_OK;

    Serial.print(F("  Username : "));
    readSerialString(username, &err);
    
    Serial.print(F("  Password : "));
    readSerialString(password, &err);

    for (int i = 0; i < totalUser; i++) {
        if (strcasecmp(daftarUser[i].username, username) == 0) {
            if (daftarUser[i].role != roleDipilih) {
                Serial.println(F("Role tidak sesuai. Masukkan role yang sesuai."));
                return;
            }
            if (strcmp(daftarUser[i].password, password) != 0) {
                Serial.println(F("Password salah!"));
                return;
            }
    
            indexUserAktif = i;
            Serial.println(F("\n Login berhasil! Selamat datang."));
            
            // Pengarahan Menu
            if (roleDipilih == ROLE_PIC) {
                process(l); 
            } else if (roleDipilih == ROLE_ADMIN) {
                menuAdmin(l); 
            } else if (roleDipilih == ROLE_PEMINJAM) {
                menuUserGeneral(l); 
            }
            
            indexUserAktif = -1; // Auto-logout jika keluar menu
            return;
        }
    }

    Serial.print(F("Username '"));
    Serial.print(username);
    Serial.println(F("' tidak ditemukan."));
}

void jalankanRegister(void) {
    Serial.println(F("\n >>REGISTER"));

    if (totalUser >= MAX_USERS) {
        Serial.println(F("[!] Kapasitas pengguna penuh."));
        return;
    }

    tampilkanPilihanRole();
    Role roleDipilih = bacaRole();
    if (roleDipilih == 0) {
        Serial.println(F("Pilihan role tidak valid."));
        return;
    }

    char username[MAX_STR], password[MAX_STR];
    ErrorCode err = ERR_OK;

    Serial.print(F("  Username : "));
    readSerialString(username, &err);

    if (usernameAda(username)) {
        Serial.println(F("Username sudah terdaftar."));
        return;
    }

    Serial.print(F("  Password : "));
    readSerialString(password, &err);

    strcpy(daftarUser[totalUser].username, username);
    strcpy(daftarUser[totalUser].password, password);
    daftarUser[totalUser].role = roleDipilih;
    totalUser++;

    Serial.println(F("Akun berhasil didaftarkan."));
}

void jalankanLupaPassword(void) {
    Serial.println(F("\n>>LUPA PASSWORD"));

    char username[MAX_STR];
    ErrorCode err = ERR_OK;
    
    Serial.print(F("  Username : "));
    readSerialString(username, &err);

    int idx = -1;
    for (int i = 0; i < totalUser; i++) {
        if (strcasecmp(daftarUser[i].username, username) == 0) {
            idx = i;
            break;
        }
    }

    if (idx == -1) {
        Serial.println(F("[!] Username tidak ditemukan."));
        return;
    }

    char encoded[MAX_STR * 2];
    runLengthEncode(encoded, daftarUser[idx].username);

    Serial.println(F("  [INFO] Kode unik = enkripsi username (Contoh: aabbc -> 2a2bc)"));
    Serial.print(F("  Masukkan kode unik : "));

    char kodeInput[MAX_STR * 2];
    readSerialString(kodeInput, &err);

    if (strcmp(kodeInput, encoded) != 0) {
        Serial.println(F("Kode unik salah!"));
        return;
    }

    char passwordBaru[MAX_STR];
    Serial.print(F("  Password Baru : "));
    readSerialString(passwordBaru, &err);

    strcpy(daftarUser[idx].password, passwordBaru);
    Serial.println(F("Password berhasil diubah."));
}

void jalankanLogout(void) {
    Serial.println(F("\n>>> LOGOUT"));
    if (indexUserAktif == -1) {
        Serial.println(F("Logout gagal."));
        return;
    }
    Serial.println(F("Sampai jumpa!"));
    indexUserAktif = -1;  
}