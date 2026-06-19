#include "inventory_system.hpp"
#include "user.hpp" // Tambahan wajib untuk membaca variabel daftarUser dan totalUser

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
// FUNGSI BARU: PENGGANTI loadInitialData
// Hanya dipanggil satu kali oleh main.cpp saat Arduino menyala
// =========================================================================
void inisialisasiSistem(InventoryList *l) {
    if (l == NULL) return;

    // 1. HARDCODE DATA AKUN AWAL
    strcpy(daftarUser[0].username, "AdminA");
    strcpy(daftarUser[0].password, "admin123");
    daftarUser[0].role = ROLE_ADMIN;

    strcpy(daftarUser[1].username, "Naya");
    strcpy(daftarUser[1].password, "Naya123");
    daftarUser[1].role = ROLE_PIC;

    strcpy(daftarUser[2].username, "Imam");
    strcpy(daftarUser[2].password, "Imam123");
    daftarUser[2].role = ROLE_PIC;
    
    totalUser = 3;

    // 2. HARDCODE 9 BARANG INVENTARIS AWAL
    ErrorCode err = ERR_OK;
    uint8_t ids[9]       = {11, 12, 13, 21, 22, 23, 31, 32, 33};
    const char* names[9] = {"Motor Servo", "Motor DC", "Relay Module", 
                            "Sensor Suhu", "Sensor Jarak", "Sensor Cahaya", 
                            "Arduino Uno", "ESP32", "STM32"};
    uint8_t cats[9]      = {KATEGORI_AKTUATOR, KATEGORI_AKTUATOR, KATEGORI_AKTUATOR,
                            KATEGORI_SENSOR, KATEGORI_SENSOR, KATEGORI_SENSOR,
                            KATEGORI_MIKROKONTROLLER, KATEGORI_MIKROKONTROLLER, KATEGORI_MIKROKONTROLLER};
    const char* pics[9]  = {"Naya", "Naya", "Naya", "Imam", "Imam", "Imam", "Naya", "Naya", "Naya"};

    for (uint8_t i = 0; i < 9; i++) {
        InventoryNode *newNode = (InventoryNode *)malloc(sizeof(InventoryNode));
        if (newNode == NULL) continue;
        
        memset(newNode, 0, sizeof(InventoryNode));
        newNode->data.itemId = ids[i];
        strcpy(newNode->data.itemName, names[i]);
        newNode->data.category = cats[i];
        newNode->data.location = LOKASI_LEMARI; 
        
        newNode->data.stock.totalStock = 3; 
        newNode->data.stock.borrowed = 0;
        newNode->data.stock.broken = 0;
        
        strcpy(newNode->data.owner, "AdminA"); 
        strcpy(newNode->data.pic, pics[i]);    
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
            // Menu 9 (Inisialisasi Data) dihapus agar PIC tidak merusak data
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