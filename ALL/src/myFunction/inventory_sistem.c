#include "inventory_system.h"
#include "user.h" 
#include "serial_bridge.h"
#include "inventory_operation.h" 
#include "ui.h"                  
#include <avr/pgmspace.h> 

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

void inisialisasiSistem(InventoryList *l) {
    if (l == NULL) return;

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
        
        // --- REVISI DIET MEMORI ---
        // Kita buang strcpy_P untuk PIC, dan ganti dengan memasukkan angkanya saja
        switch(i) {
            case 0:
                strcpy_P(newNode->data.itemName, PSTR("Motor Servo"));
                newNode->data.picIndex = 1; // 1 = Naya
                break;
            case 1:
                strcpy_P(newNode->data.itemName, PSTR("Motor DC"));
                newNode->data.picIndex = 1;
                break;
            case 2:
                strcpy_P(newNode->data.itemName, PSTR("Relay Module"));
                newNode->data.picIndex = 1;
                break;
            case 3:
                strcpy_P(newNode->data.itemName, PSTR("Sensor Suhu"));
                newNode->data.picIndex = 2; // 2 = Imam
                break;
            case 4:
                strcpy_P(newNode->data.itemName, PSTR("Sensor Jarak"));
                newNode->data.picIndex = 2;
                break;
            case 5:
                strcpy_P(newNode->data.itemName, PSTR("Sensor Cahaya"));
                newNode->data.picIndex = 2;
                break;
            case 6:
                strcpy_P(newNode->data.itemName, PSTR("Arduino Uno"));
                newNode->data.picIndex = 1;
                break;
            case 7:
                strcpy_P(newNode->data.itemName, PSTR("ESP32"));
                newNode->data.picIndex = 1;
                break;
            case 8:
                strcpy_P(newNode->data.itemName, PSTR("STM32"));
                newNode->data.picIndex = 1;
                break;
        }
        
        // Owner juga diganti jadi angka index saja (0 = AdminA)
        newNode->data.ownerIndex = 0; 
        // ---------------------------
        
        newNode->next = NULL;
        insertNodeToList(l, newNode, &err);
    }

    serial_cetak_teks_ln_flash(PSTR("\n=================================="));
    serial_cetak_teks_ln_flash(PSTR(" SISTEM INVENTARIS LAB MENYALA! "));
    serial_cetak_teks_ln_flash(PSTR("=================================="));
}

void process(InventoryList *l) {
    if (l == NULL) {
        ERROR_FLAG = ERR_DATABASE_NULL;
        return;
    }

    bool isPICLoggedIn = true;
    shouldDisplayMenu = true; 

    while (isPICLoggedIn) {
        if (shouldDisplayMenu) {
            serial_cetak_teks_ln_flash(PSTR("\n====== MENU INVENTARIS (PIC) ======"));
            serial_cetak_teks_ln_flash(PSTR("3. Mencari Data"));
            serial_cetak_teks_ln_flash(PSTR("4. Memperbarui Stock"));
            serial_cetak_teks_ln_flash(PSTR("5. Memperbarui Data"));
            serial_cetak_teks_ln_flash(PSTR("6. Menampilkan Semua Data"));
            serial_cetak_teks_ln_flash(PSTR("7. Menghapus Semua Data"));
            serial_cetak_teks_ln_flash(PSTR("8. Menampilkan Sisa SRAM"));
            serial_cetak_teks_ln_flash(PSTR("0. Logout"));
            serial_cetak_teks_ln_flash(PSTR("==================================="));
            serial_cetak_teks_flash(PSTR("Pilih menu: "));
            shouldDisplayMenu = false;
        }

        char buffer[10];
        serial_baca_string(buffer, 10);
        char choice = buffer[0];
        
        ERROR_FLAG = ERR_OK;

        switch (choice) {
            case '3': {
                runInventoryMenu(l, CMD_SEARCH, &ERROR_FLAG);
                if (ERROR_FLAG != ERR_OK) printErrorMessage(ERROR_FLAG);
                shouldDisplayMenu = true; serial_bersihkan(); break;
            }
            case '4': {
                runInventoryMenu(l, CMD_UPDATE_STOCK, &ERROR_FLAG);
                if (ERROR_FLAG != ERR_OK) printErrorMessage(ERROR_FLAG);
                shouldDisplayMenu = true; serial_bersihkan(); break;
            }
            case '5': {
                runInventoryMenu(l, CMD_UPDATE_ITEM, &ERROR_FLAG);
                if (ERROR_FLAG != ERR_OK) printErrorMessage(ERROR_FLAG);
                shouldDisplayMenu = true; serial_bersihkan(); break;
            }
            case '6': {
                displayList(l, &ERROR_FLAG);
                if (ERROR_FLAG != ERR_OK) printErrorMessage(ERROR_FLAG);
                shouldDisplayMenu = true; serial_bersihkan(); break;
            }
            case '7': {
                destroyList(l, &ERROR_FLAG);
                if (ERROR_FLAG != ERR_OK) printErrorMessage(ERROR_FLAG);
                shouldDisplayMenu = true; serial_bersihkan(); break;
            }
            case '8': {
                printMemory(l);
                shouldDisplayMenu = true; serial_bersihkan(); break;
            }
            case '0': { 
                serial_cetak_teks_ln_flash(PSTR("\nLogout berhasil. Kembali ke menu utama..."));
                isPICLoggedIn = false; serial_bersihkan(); break;
            }
            default: {
                if (choice != '\n' && choice != '\r' && choice != '\0') {
                    serial_cetak_teks_ln_flash(PSTR("\nPilihan salah atau Anda tidak memiliki hak akses!"));
                    shouldDisplayMenu = true;
                }
                break;
            }
        }
    }
}