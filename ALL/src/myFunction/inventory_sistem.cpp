#include "inventory_system.hpp"
#include "user.hpp" // Tambahan wajib untuk membaca variabel daftarUser dan totalUser
#include <avr/pgmspace.h> // Wajib untuk menggunakan fungsi PROGMEM / PSTR

ErrorCode ERROR_FLAG = ERR_OK;
bool shouldDisplayMenu = true;

void createList(InventoryList *l) {
    if (l == NULL) {
        ERROR_FLAG = ERR_DATABASE_NULL;
        return;
    }
    l->head = NULL;
    l->size = 0;
    ERROR_FLAG = ERR_OK;
}

// =========================================================================
// FUNGSI INISIALISASI (VERSI OPTIMASI MEMORI TINGKAT TINGGI)
// =========================================================================
void inisialisasiSistem(InventoryList *l) {
    if (l == NULL) return;

    // 1. HARDCODE DATA AKUN AWAL (Menggunakan PSTR agar hemat RAM)
    strcpy_P(daftarUser[0].username, PSTR("AdminA"));
    strcpy_P(daftarUser[0].password, PSTR("admin123"));
    daftarUser[0].role = ROLE_ADMIN;

    strcpy_P(daftarUser[1].username, PSTR("Naya"));
    strcpy_P(daftarUser[1].password, PSTR("Naya123"));
    daftarUser[1].role = ROLE_PIC;

    strcpy_P(daftarUser[2].username, PSTR("Imam"));
    strcpy_P(daftarUser[2].password, PSTR("Imam123"));
    daftarUser[2].role = ROLE_PIC;
    
    totalUser = 3;

    // 2. HARDCODE 9 BARANG INVENTARIS AWAL
    ErrorCode err = ERR_OK;
    uint8_t ids[9]       = {11, 12, 13, 21, 22, 23, 31, 32, 33};
    uint8_t cats[9]      = {KATEGORI_AKTUATOR, KATEGORI_AKTUATOR, KATEGORI_AKTUATOR,
                            KATEGORI_SENSOR, KATEGORI_SENSOR, KATEGORI_SENSOR,
                            KATEGORI_MIKROKONTROLLER, KATEGORI_MIKROKONTROLLER, KATEGORI_MIKROKONTROLLER};

    for (uint8_t i = 0; i < 9; i++) {
        InventoryNode *newNode = (InventoryNode *)malloc(sizeof(InventoryNode));
        if (newNode == NULL) continue;
        
        memset(newNode, 0, sizeof(InventoryNode));
        newNode->data.itemId = ids[i];
        newNode->data.category = cats[i];
        newNode->data.location = LOKASI_LEMARI; 
        
        newNode->data.stock.totalStock = 3; 
        newNode->data.stock.borrowed = 0;
        newNode->data.stock.broken = 0;
        
        // Suntikkan teks langsung dari Flash Memory ke Node menggunakan switch-case
        switch(i) {
            case 0:
                strcpy_P(newNode->data.itemName, PSTR("Motor Servo"));
                strcpy_P(newNode->data.pic, PSTR("Naya"));
                break;
            case 1:
                strcpy_P(newNode->data.itemName, PSTR("Motor DC"));
                strcpy_P(newNode->data.pic, PSTR("Naya"));
                break;
            case 2:
                strcpy_P(newNode->data.itemName, PSTR("Relay Module"));
                strcpy_P(newNode->data.pic, PSTR("Naya"));
                break;
            case 3:
                strcpy_P(newNode->data.itemName, PSTR("Sensor Suhu"));
                strcpy_P(newNode->data.pic, PSTR("Imam"));
                break;
            case 4:
                strcpy_P(newNode->data.itemName, PSTR("Sensor Jarak"));
                strcpy_P(newNode->data.pic, PSTR("Imam"));
                break;
            case 5:
                strcpy_P(newNode->data.itemName, PSTR("Sensor Cahaya"));
                strcpy_P(newNode->data.pic, PSTR("Imam"));
                break;
            case 6:
                strcpy_P(newNode->data.itemName, PSTR("Arduino Uno"));
                strcpy_P(newNode->data.pic, PSTR("Naya"));
                break;
            case 7:
                strcpy_P(newNode->data.itemName, PSTR("ESP32"));
                strcpy_P(newNode->data.pic, PSTR("Naya"));
                break;
            case 8:
                strcpy_P(newNode->data.itemName, PSTR("STM32"));
                strcpy_P(newNode->data.pic, PSTR("Naya"));
                break;
        }
        
        // Semua barang ini sumbangan AdminA di awal
        strcpy_P(newNode->data.owner, PSTR("AdminA")); 
        
        newNode->next = NULL;
        insertNodeToList(l, newNode, &err);
    }

    Serial.println(F("\n=================================="));
    Serial.println(F(" SISTEM INVENTARIS LAB MENYALA! "));
    Serial.println(F("=================================="));
}

// =========================================================================
// MENU KHUSUS PIC
// =========================================================================
void process(InventoryList *l) {
    if (l == NULL) {
        ERROR_FLAG = ERR_DATABASE_NULL;
        return;
    }

    bool isPICLoggedIn = true;
    shouldDisplayMenu = true; 

    while (isPICLoggedIn) {
        if (shouldDisplayMenu) {
            Serial.println(F("\n====== MENU INVENTARIS (PIC) ======"));
            Serial.println(F("3. Mencari Data"));
            Serial.println(F("4. Memperbarui Stock"));
            Serial.println(F("5. Memperbarui Data"));
            Serial.println(F("6. Menampilkan Semua Data"));
            Serial.println(F("7. Menghapus Semua Data"));
            Serial.println(F("8. Menampilkan Sisa SRAM"));
            // Menu 9 dihapus agar PIC tidak merusak data awal
            Serial.println(F("0. Logout"));
            Serial.println(F("==================================="));
            Serial.print(F("Pilih menu: "));
            shouldDisplayMenu = false;
        }

        while (!Serial.available()) {
            delay(10); 
        }

        char choice = Serial.read();
        ERROR_FLAG = ERR_OK;

        switch (choice) {
            case '3': {
                runInventoryMenu(l, CMD_SEARCH, &ERROR_FLAG);
                if (ERROR_FLAG != ERR_OK) printErrorMessage(ERROR_FLAG);
                shouldDisplayMenu = true; clearSerial(); break;
            }
            case '4': {
                runInventoryMenu(l, CMD_UPDATE_STOCK, &ERROR_FLAG);
                if (ERROR_FLAG != ERR_OK) printErrorMessage(ERROR_FLAG);
                shouldDisplayMenu = true; clearSerial(); break;
            }
            case '5': {
                runInventoryMenu(l, CMD_UPDATE_ITEM, &ERROR_FLAG);
                if (ERROR_FLAG != ERR_OK) printErrorMessage(ERROR_FLAG);
                shouldDisplayMenu = true; clearSerial(); break;
            }
            case '6': {
                displayList(l, &ERROR_FLAG);
                if (ERROR_FLAG != ERR_OK) printErrorMessage(ERROR_FLAG);
                shouldDisplayMenu = true; clearSerial(); break;
            }
            case '7': {
                destroyList(l, &ERROR_FLAG);
                if (ERROR_FLAG != ERR_OK) printErrorMessage(ERROR_FLAG);
                shouldDisplayMenu = true; clearSerial(); break;
            }
            case '8': {
                printMemory(l);
                shouldDisplayMenu = true; clearSerial(); break;
            }
            case '0': { 
                Serial.println(F("\nLogout berhasil. Kembali ke menu utama..."));
                isPICLoggedIn = false; clearSerial(); break;
            }
            default: {
                if (choice != '\n' && choice != '\r') {
                    Serial.println(F("\nPilihan salah atau Anda tidak memiliki hak akses!"));
                    shouldDisplayMenu = true;
                }
                break;
            }
        }
    }
}