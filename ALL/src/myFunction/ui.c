#include "ui.h"
#include "serial_bridge.h"
#include "serial_utils.h" 
#include "helper.h"
#include "memory_management.h"
#include "user.h" 
#include <string.h>

// Ambil variabel global dari file user/login
extern User daftarUser[];

void displayMenu() {
    serial_cetak_teks_ln_flash(PSTR("\n====== MENU INVENTARIS ======"));
    serial_cetak_teks_ln_flash(PSTR("1. Menambah Data"));
    serial_cetak_teks_ln_flash(PSTR("2. Menghapus Data"));
    serial_cetak_teks_ln_flash(PSTR("3. Mencari Data"));
    serial_cetak_teks_ln_flash(PSTR("4. Memperbarui Stock"));
    serial_cetak_teks_ln_flash(PSTR("5. Memperbarui Data"));
    serial_cetak_teks_ln_flash(PSTR("6. Menampilkan Semua Data"));
    serial_cetak_teks_ln_flash(PSTR("7. Menghapus Semua Data"));
    serial_cetak_teks_ln_flash(PSTR("8. Menampilkan Sisa SRAM"));
    serial_cetak_teks_ln_flash(PSTR("9. Inisialisasi Data"));
    serial_cetak_teks_ln_flash(PSTR("============================="));
    serial_cetak_teks_flash(PSTR("Pilih menu: "));
}

void displayList(const InventoryList *l, ErrorCode *err) {
    if (l == NULL) {
        *err = ERR_DATABASE_NULL;
        return;
    }

    if (l->head == NULL) {
        serial_cetak_teks_ln_flash(PSTR("\nDatabase kosong, tidak ada data untuk ditampilkan"));
        *err = ERR_OK;
        return;
    }

    serial_cetak_teks_ln_flash(PSTR("\n===== DATA INVENTORY ====="));

    InventoryNode *curr = l->head;

    while (curr != NULL) {
        displayItemData(&curr->data);
        curr = curr->next;
    }

    *err = ERR_OK;
}

void displayItemData(const ItemData *item) {
    if (item == NULL) {
        serial_cetak_teks_ln_flash(PSTR("Data item NULL"));
        return;
    }

    uint8_t availableStock = 0;
    getAvailableStock(&item->stock, &availableStock);

    serial_cetak_teks_flash(PSTR("Item ID       : "));
    serial_cetak_angka_ln(item->itemId);

    serial_cetak_teks_flash(PSTR("Nama          : "));
    serial_cetak_teks_ln(item->itemName);

    serial_cetak_teks_flash(PSTR("Kategori      : "));
    printCategory((ItemCategory)item->category);

    serial_cetak_teks_flash(PSTR("Lokasi        : "));
    printLocation((ItemLocation)item->location);

    serial_cetak_teks_flash(PSTR("Total Stock   : "));
    serial_cetak_angka_ln(item->stock.totalStock);

    serial_cetak_teks_flash(PSTR("Tersedia      : "));
    serial_cetak_angka_ln(availableStock);

    serial_cetak_teks_flash(PSTR("Dipinjam      : "));
    serial_cetak_angka_ln(item->stock.borrowed);

    serial_cetak_teks_flash(PSTR("Rusak         : "));
    serial_cetak_angka_ln(item->stock.broken);

    serial_cetak_teks_flash(PSTR("Pemilik       : "));
    if (item->ownerIndex >= 0 && item->ownerIndex < MAX_USERS) {
        serial_cetak_teks_ln(daftarUser[item->ownerIndex].username);
    } else {
        serial_cetak_teks_ln_flash(PSTR("Tidak Diketahui"));
    }

    serial_cetak_teks_flash(PSTR("PIC           : "));
    if (item->picIndex >= 0 && item->picIndex < MAX_USERS) {
        serial_cetak_teks_ln(daftarUser[item->picIndex].username);
    } else {
        serial_cetak_teks_ln_flash(PSTR("Tidak Diketahui"));
    }
    // ---------------------------

    serial_cetak_teks_ln_flash(PSTR("------------------------"));
}

void printErrorMessage(const ErrorCode err) {
    serial_cetak_teks_flash(PSTR("\n[ERROR] "));
    switch (err) {
        case ERR_OK:                        serial_cetak_teks_ln_flash(PSTR("OK")); break;
        case ERR_DATA_NULL:                 serial_cetak_teks_ln_flash(PSTR("Data NULL")); break;
        case ERR_DATABASE_NULL:             serial_cetak_teks_ln_flash(PSTR("Database NULL")); break;
        case ERR_NODE_ALLOCATION_FAILED:    serial_cetak_teks_ln_flash(PSTR("Alokasi node gagal")); break;
        case ERR_INDEX_INVALID:             serial_cetak_teks_ln_flash(PSTR("Index tidak valid")); break;
        case ERR_LIST_FULL:                 serial_cetak_teks_ln_flash(PSTR("List penuh")); break;
        case ERR_SERIAL_INPUT_TIMEOUT:      serial_cetak_teks_ln_flash(PSTR("Timeout input serial")); break;
        case ERR_STRING_EMPTY:              serial_cetak_teks_ln_flash(PSTR("String kosong")); break;
        case ERR_NON_UNSIGNED_NUMERIC:      serial_cetak_teks_ln_flash(PSTR("Input bukan bilangan bulat positif")); break;
        case ERR_OVERFLOW_UINT8:            serial_cetak_teks_ln_flash(PSTR("Overflow uint8")); break;
        case ERR_ID_EMPTY:                  serial_cetak_teks_ln_flash(PSTR("ID kosong")); break;
        case ERR_STOCK_EMPTY:               serial_cetak_teks_ln_flash(PSTR("Stock kosong")); break;
        case ERR_NAME_EMPTY:                serial_cetak_teks_ln_flash(PSTR("Nama kosong")); break;
        case ERR_OWNER_EMPTY:               serial_cetak_teks_ln_flash(PSTR("Pemilik kosong")); break;
        case ERR_PIC_EMPTY:                 serial_cetak_teks_ln_flash(PSTR("PIC kosong")); break;
        default:                            serial_cetak_teks_ln_flash(PSTR("Error Tidak Diketahui")); break;
    }
}

void printCategory(ItemCategory cat) {
    switch (cat) {
        case KATEGORI_SENSOR:          serial_cetak_teks_ln_flash(PSTR("Sensor")); break;
        case KATEGORI_AKTUATOR:        serial_cetak_teks_ln_flash(PSTR("Aktuator")); break;
        case KATEGORI_MIKROKONTROLLER: serial_cetak_teks_ln_flash(PSTR("Mikrokontroller")); break;
        default:                       serial_cetak_teks_ln_flash(PSTR("Kategori Tidak Diketahui")); break;
    }
}

void printLocation(ItemLocation loc) {
    switch (loc) {
        case LOKASI_LEMARI: serial_cetak_teks_ln_flash(PSTR("Lemari")); break;
        case LOKASI_MEJA:   serial_cetak_teks_ln_flash(PSTR("Meja")); break;
        default:            serial_cetak_teks_ln_flash(PSTR("Lokasi Tidak Diketahui")); break;
    }
}

void printMemory(const InventoryList *l) {
    int freeRam = 0;

    getFreeRam(&freeRam);

    serial_cetak_teks_ln_flash(PSTR(""));

    serial_cetak_teks_flash(PSTR("Total Data      : "));
    serial_cetak_angka_ln(l->size);

    serial_cetak_teks_flash(PSTR("Ukuran Node     : "));
    serial_cetak_angka((int)sizeof(InventoryNode));
    serial_cetak_teks_ln_flash(PSTR(" bytes"));

    serial_cetak_teks_flash(PSTR("RAM Tersisa     : "));
    serial_cetak_angka(freeRam);
    serial_cetak_teks_ln_flash(PSTR(" bytes"));
}

void getInput(InventoryList *l, InventoryNode *newNode, ErrorCode *err) {
    char buffer[MAX_LENGTH + 1]; 
    uint8_t value = 0;

    serial_bersihkan();
    serial_cetak_teks_ln_flash(PSTR("\n===== INPUT BARANG ====="));

    serial_cetak_teks_flash(PSTR("ID Barang : "));
    readSerialString(buffer, err);
    if (*err != ERR_OK) return;

    stringToInt(buffer, &value, err);
    if (*err != ERR_OK) {
        if (*err == ERR_STRING_EMPTY) *err = ERR_ID_EMPTY;
        return;
    }

    bool isExist = false;
    isItemIdExist(l, value, &isExist, err);

    if (isExist) {
        serial_cetak_teks_ln_flash(PSTR("\nID barang telah dipakai"));
        *err = ERR_OK;
        return;
    }

    newNode->data.itemId = value;

    serial_cetak_teks_flash(PSTR("Nama Barang : "));
    readSerialString(newNode->data.itemName, err);
    if (*err != ERR_OK) return;

    if (strlen(newNode->data.itemName) == 0) {
        *err = ERR_NAME_EMPTY;
        return;
    }

    serial_cetak_teks_ln_flash(PSTR("\nDaftar Kategori"));
    for (uint8_t i = 0; i < KATEGORI_COUNT; i++) {
        serial_cetak_angka(i);
        serial_cetak_teks_flash(PSTR(". "));
        printCategory((ItemCategory)i);
    }

    serial_cetak_teks_flash(PSTR("Pilih kategori : "));
    readSerialString(buffer, err);
    if (*err != ERR_OK) return;

    stringToInt(buffer, &value, err);
    if (*err != ERR_OK || value >= KATEGORI_COUNT) {
        serial_cetak_teks_ln_flash(PSTR("Pilihan Tidak Ada"));
        *err = ERR_OK;
        return;
    }

    newNode->data.category = value;

    serial_cetak_teks_ln_flash(PSTR("\nDaftar Lokasi"));
    for (uint8_t i = 0; i < LOKASI_COUNT; i++) {
        serial_cetak_angka(i);
        serial_cetak_teks_flash(PSTR(". "));
        printLocation((ItemLocation)i);
    }

    serial_cetak_teks_flash(PSTR("Pilih lokasi : "));
    readSerialString(buffer, err);
    if (*err != ERR_OK) return;

    stringToInt(buffer, &value, err);
    if (*err != ERR_OK || value >= LOKASI_COUNT) {
        serial_cetak_teks_ln_flash(PSTR("Pilihan Tidak Ada"));
        *err = ERR_OK;
        return;
    }

    newNode->data.location = value;

    serial_cetak_teks_flash(PSTR("\nJumlah Stock : "));
    readSerialString(buffer, err);
    if (*err != ERR_OK) return;

    stringToInt(buffer, &value, err);
    if (*err != ERR_OK) {
        if (*err == ERR_STRING_EMPTY) *err = ERR_STOCK_EMPTY;
        return;
    }

    newNode->data.stock.totalStock = value;
    newNode->data.stock.borrowed = 0;
    newNode->data.stock.broken = 0;


    newNode->data.ownerIndex = -1; 
    newNode->data.picIndex = -1;
    // ---------------------------

    *err = ERR_OK;
}