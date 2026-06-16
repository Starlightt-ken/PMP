#include "ui.hpp"

void displayMenu() {
    Serial.println(F("\n====== MENU INVENTARIS ======"));
    Serial.println(F("1. Menambah Data"));
    Serial.println(F("2. Menghapus Data"));
    Serial.println(F("3. Mencari Data"));
    Serial.println(F("4. Memperbarui Stock"));
    Serial.println(F("5. Memperbarui Data"));
    Serial.println(F("6. Menampilkan Semua Data"));
    Serial.println(F("7. Menghapus Semua Data"));
    Serial.println(F("8. Menampilkan Sisa SRAM"));
    Serial.println(F("9. Inisialisasi Data"));
    Serial.println(F("============================="));
    Serial.print(F("Pilih menu: "));
}

void displayList(const InventoryList *l, ErrorCode *err) {
    if (l == NULL) {
        *err = ERR_DATABASE_NULL;
        return;
    }

    if (l->head == NULL) {
        Serial.println(F("\nDatabase kosong, tidak ada data untuk ditampilkan"));
        *err = ERR_OK;
        return;
    }

    Serial.println(F("\n===== DATA INVENTORY ====="));

    InventoryNode *curr = l->head;

    while (curr != NULL) {
        displayItemData(&curr->data);
        curr = curr->next;
    }

    *err = ERR_OK;
}

void displayItemData(const ItemData *item) {
    if (item == NULL) {
        Serial.println(F("Data item NULL"));
        return;
    }

    uint8_t availableStock = 0;
    getAvailableStock(&item->stock, &availableStock);

    Serial.print(F("Item ID       : "));
    Serial.println(item->itemId);

    Serial.print(F("Nama          : "));
    Serial.println(item->itemName);

    Serial.print(F("Kategori      : "));
    printCategory((ItemCategory)item->category);

    Serial.print(F("Lokasi        : "));
    printLocation((ItemLocation)item->location);

    Serial.print(F("Total Stock   : "));
    Serial.println(item->stock.totalStock);

    Serial.print(F("Tersedia      : "));
    Serial.println(availableStock);

    Serial.print(F("Dipinjam      : "));
    Serial.println(item->stock.borrowed);

    Serial.print(F("Rusak         : "));
    Serial.println(item->stock.broken);

    Serial.print(F("Pemilik       : "));
    Serial.println(item->owner);

    Serial.print(F("PIC           : "));
    Serial.println(item->pic);

    Serial.println(F("------------------------"));
}

void printErrorMessage(const ErrorCode err) {
    Serial.print(F("\n[ERROR] "));
    switch (err) {
        case ERR_OK:                        Serial.println(F("OK")); break;
        case ERR_DATA_NULL:                 Serial.println(F("Data NULL")); break;
        case ERR_DATABASE_NULL:             Serial.println(F("Database NULL")); break;
        case ERR_NODE_ALLOCATION_FAILED:    Serial.println(F("Alokasi node gagal")); break;
        case ERR_INDEX_INVALID:             Serial.println(F("Index tidak valid")); break;
        case ERR_LIST_FULL:                 Serial.println(F("List penuh")); break;
        case ERR_SERIAL_INPUT_TIMEOUT:      Serial.println(F("Timeout input serial")); break;
        case ERR_STRING_EMPTY:              Serial.println(F("String kosong")); break;
        case ERR_NON_UNSIGNED_NUMERIC:      Serial.println(F("Input bukan bilangan bulat positif")); break;
        case ERR_OVERFLOW_UINT8:            Serial.println(F("Overflow uint8")); break;
        case ERR_ID_EMPTY:                  Serial.println(F("ID kosong")); break;
        case ERR_STOCK_EMPTY:               Serial.println(F("Stock kosong")); break;
        case ERR_NAME_EMPTY:                Serial.println(F("Nama kosong")); break;
        case ERR_OWNER_EMPTY:               Serial.println(F("Pemilik kosong")); break;
        case ERR_PIC_EMPTY:                 Serial.println(F("PIC kosong")); break;
        default:                            Serial.println(F("Error Tidak Diketahui")); break;
    }
}

void printCategory(ItemCategory cat) {
    switch (cat) {
        case KATEGORI_SENSOR:          Serial.println(F("Sensor")); break;
        case KATEGORI_AKTUATOR:        Serial.println(F("Aktuator")); break;
        case KATEGORI_MIKROKONTROLLER: Serial.println(F("Mikrokontroller")); break;
        default:                       Serial.println(F("Kategori Tidak Diketahui")); break;
    }
}

void printLocation(ItemLocation loc) {
    switch (loc) {
        case LOKASI_LEMARI: Serial.println(F("Lemari")); break;
        case LOKASI_MEJA:   Serial.println(F("Meja")); break;
        default:            Serial.println(F("Lokasi Tidak Diketahui")); break;
    }
}

void printMemory(const InventoryList *l) {
    int freeRam;

    getFreeRam(freeRam);

    Serial.println();

    Serial.print(F("Total Data      : "));
    Serial.println(l->size);

    Serial.print(F("Ukuran Node     : "));
    Serial.print((size_t)sizeof(InventoryNode));
    Serial.println(F(" bytes"));

    Serial.print(F("RAM Tersisa     : "));
    Serial.print(freeRam);
    Serial.println(F(" bytes"));
}

void getInput(InventoryList *l, InventoryNode *newNode, ErrorCode *err) {
    char buffer[MAX_LENGTH + 1]; 
    uint8_t value = 0;

    clearSerial();
    Serial.println(F("\n===== INPUT BARANG ====="));

    // ID
    Serial.print(F("ID Barang : "));
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
        Serial.println(F("\nID barang telah dipakai"));
        *err = ERR_OK;
        return;
    }

    newNode->data.itemId = value;

    // Nama
    Serial.print(F("Nama Barang : "));
    readSerialString(newNode->data.itemName, err);
    if (*err != ERR_OK) return;

    if (strlen(newNode->data.itemName) == 0) {
        *err = ERR_NAME_EMPTY;
        return;
    }

    // Kategori
    Serial.println(F("\nDaftar Kategori"));
    for (uint8_t i = 0; i < KATEGORI_COUNT; i++) {
        Serial.print(i);
        Serial.print(F(". "));
        printCategory((ItemCategory)i);
    }

    Serial.print(F("Pilih kategori : "));
    readSerialString(buffer, err);
    if (*err != ERR_OK) return;

    stringToInt(buffer, &value, err);
    if (*err != ERR_OK || value >= KATEGORI_COUNT) {
        Serial.println(F("Pilihan Tidak Ada"));
        *err = ERR_OK;
        return;
    }

    newNode->data.category = value;

    // Lokasi
    Serial.println(F("\nDaftar Lokasi"));
    for (uint8_t i = 0; i < LOKASI_COUNT; i++) {
        Serial.print(i);
        Serial.print(F(". "));
        printLocation((ItemLocation)i);
    }

    Serial.print(F("Pilih lokasi : "));
    readSerialString(buffer, err);
    if (*err != ERR_OK) return;

    stringToInt(buffer, &value, err);
    if (*err != ERR_OK || value >= LOKASI_COUNT) {
        Serial.println(F("Pilihan Tidak Ada"));
        *err = ERR_OK;
        return;
    }

    newNode->data.location = value;

    // Stock
    Serial.print(F("\nJumlah Stock : "));
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

    // Owner
    Serial.print(F("Pemilik : "));
    readSerialString(newNode->data.owner, err);
    if (*err != ERR_OK) return;

    if (strlen(newNode->data.owner) == 0) {
        *err = ERR_OWNER_EMPTY;
        return;
    }

    // PIC
    Serial.print(F("PIC : "));
    readSerialString(newNode->data.pic, err);
    if (*err != ERR_OK) return;

    if (strlen(newNode->data.pic) == 0) {
        *err = ERR_PIC_EMPTY;
        return;
    }

    *err = ERR_OK;
}