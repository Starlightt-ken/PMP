#include "ui.h"
#include "serial_bridge.h"
#include "serial_utils.h" 
#include "helper.h"
#include "memory_management.h"
#include "user.h" 
#include <string.h>

// Ambil variabel global dari file user/login
extern User daftarUser[];

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

    serial_cetak_teks_flash(PSTR("RAM Tersisa     : "));
    serial_cetak_angka(freeRam);
    serial_cetak_teks_ln_flash(PSTR(" bytes"));
}