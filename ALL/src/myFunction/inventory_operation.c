#include "inventory_operation.h"
#include "serial_bridge.h"
#include <stdlib.h>
#include <string.h>

void destroyList(InventoryList *l, ErrorCode *err) {
    destroyListCore(l, err);

    if (*err == ERR_DATABASE_NULL) {
        return;
    }
    else if (*err == ERR_LIST_EMPTY) {
        serial_cetak_teks_ln_flash(PSTR("\nDatabase kosong, tidak ada data untuk dihapus"));
    } 
    else {
        serial_cetak_teks_ln_flash(PSTR("Semua data berhasil dihapus"));
    }
}

void insertList(InventoryList *l, ErrorCode *err) {
    if (l == NULL) {
        *err = ERR_DATABASE_NULL;
        return;
    }

    if (l->size >= MAX_CAPACITY) {
        *err = ERR_LIST_FULL;
        return;
    }

    InventoryNode *newNode = (InventoryNode*) malloc(sizeof(InventoryNode));
    if (newNode == NULL) {
        *err = ERR_NODE_ALLOCATION_FAILED;
        serial_cetak_teks_ln_flash(PSTR("Gagal alokasi node"));
        return;
    }

    memset(newNode, 0, sizeof(InventoryNode));
    newNode->next = NULL;

    getInput(l, newNode, err);

    if (*err != ERR_OK) {
        free(newNode);
        return;
    }

    insertNodeToList(l, newNode, err);

    if (*err != ERR_OK) {
        free(newNode);
        return;
    }

    serial_cetak_teks_ln_flash(PSTR("\n-- Rangkuman Data Baru --"));
    displayItemData(&newNode->data);
    serial_cetak_teks_ln_flash(PSTR("BERHASIL DITAMBAHKAN"));
}

void runInventoryMenu(InventoryList *l, MenuCommand command, ErrorCode *err) {
    *err = ERR_OK;

    if (l == NULL) {
        *err = ERR_DATABASE_NULL;
        return;
    }

    if (l->head == NULL) {
        serial_cetak_teks_ln_flash(PSTR("\nDatabase kosong"));
        *err = ERR_OK;
        return;
    }

    serial_bersihkan(); // Pengganti clearSerial()

    switch (command) {
        case CMD_SEARCH: 
            serial_cetak_teks_ln_flash(PSTR("\n=== CARI DATA BARANG ===")); break;
        case CMD_DELETE: 
            serial_cetak_teks_ln_flash(PSTR("\n=== HAPUS DATA BARANG ===")); break;
        case CMD_UPDATE_ITEM: 
            serial_cetak_teks_ln_flash(PSTR("\n=== UPDATE DATA BARANG ===")); break;
        case CMD_UPDATE_STOCK: 
            serial_cetak_teks_ln_flash(PSTR("\n=== UPDATE STOK BARANG ===")); break;
    }

    serial_cetak_teks_ln_flash(PSTR("1. Berdasarkan ID"));
    serial_cetak_teks_ln_flash(PSTR("2. Berdasarkan Nama"));
    serial_cetak_teks_flash(PSTR("Pilih metode: "));

    char buffer[MAX_LENGTH + 1];

    readSerialString(buffer, err);
    if (*err != ERR_OK) return;

    char choice = buffer[0];
    serial_bersihkan();

    if (choice == '1') {
        serial_cetak_teks_flash(PSTR("Masukkan ID Barang: "));
        readSerialString(buffer, err);
        if (*err != ERR_OK) return;

        uint8_t targetId = 0;
        stringToInt(buffer, &targetId, err);
        if (*err != ERR_OK) return;

        processById(l, targetId, command, err);
        if (*err != ERR_OK) return;
    }
    else if (choice == '2') {
        serial_cetak_teks_flash(PSTR("Masukkan Nama Barang: "));
        readSerialString(buffer, err);
        if (*err != ERR_OK) return;

        processByName(l, buffer, command, err);
        if (*err != ERR_OK) return;
    }
    else {
        serial_cetak_teks_ln_flash(PSTR("Pilihan Tidak Ada"));
    }
    *err = ERR_OK;
}

void processById(InventoryList *l, uint8_t targetId, MenuCommand command, ErrorCode *err) {
    InventoryNode *curr = l->head;
    InventoryNode *prev = NULL;

    while (curr != NULL) {
        if (curr->data.itemId == targetId) {

            switch (command) {
                case CMD_SEARCH: {
                    serial_cetak_teks_ln_flash(PSTR("--- BARANG DITEMUKAN ---"));
                    displayItemData(&curr->data);
                    break;
                }
                case CMD_DELETE: {
                    deleteInventoryNode(l, curr, prev);
                    serial_cetak_teks_flash(PSTR("\nBarang dengan ID "));
                    serial_cetak_angka(targetId);
                    serial_cetak_teks_ln_flash(PSTR(" BERHASIL DIHAPUS"));
                    break;
                }
                case CMD_UPDATE_ITEM: {
                    serial_cetak_teks_flash(PSTR("\nBarang Ditemukan: "));
                    serial_cetak_teks_ln(curr->data.itemName);
                    modifyItemInventoryNode(curr, err);
                    break;
                }
                case CMD_UPDATE_STOCK: {
                    serial_cetak_teks_flash(PSTR("\nBarang Ditemukan: "));
                    serial_cetak_teks_ln(curr->data.itemName);
                    modifyStockInventoryNode(curr, err);
                    break;
                }
            }
            return;
        }
        prev = curr;
        curr = curr->next;
    }
    serial_cetak_teks_ln_flash(PSTR("\nBarang dengan ID tersebut tidak ditemukan."));
}

void processByName(InventoryList *l, const char *targetName, MenuCommand command, ErrorCode *err) {
    if (command == CMD_SEARCH) {
        serial_cetak_teks_flash(PSTR("\nMencari Nama: "));
        serial_cetak_teks_ln(targetName);

        bool found = false;
        for (InventoryNode *curr = l->head; curr != NULL; curr = curr->next) {
            if (strcmp(curr->data.itemName, targetName) == 0) {
                if (!found) serial_cetak_teks_ln_flash(PSTR("--- BARANG DITEMUKAN ---"));
                displayItemData(&curr->data);
                found = true;
            }
        }

        if (!found) {
            serial_cetak_teks_ln_flash(PSTR("Barang dengan nama tersebut tidak ditemukan."));
        }
        *err = ERR_OK;
        return;
    }

    bool foundAny = false;
    char buffer[10];

    InventoryNode *curr = l->head;
    InventoryNode *prev = NULL;

    while (curr != NULL) {
        if (strcmp(curr->data.itemName, targetName) == 0) {
            foundAny = true;

            serial_cetak_teks_ln_flash(PSTR("\nBarang Ditemukan:"));
            serial_cetak_teks_flash(PSTR("- ID       : ")); serial_cetak_angka_ln(curr->data.itemId);
            serial_cetak_teks_flash(PSTR("- Kategori : ")); printCategory((ItemCategory)curr->data.category);
            serial_cetak_teks_flash(PSTR("- Lokasi   : ")); printLocation((ItemLocation)curr->data.location);
            serial_cetak_teks_flash(PSTR("- Pemilik  : ")); serial_cetak_teks_ln(curr->data.owner);
            serial_cetak_teks_flash(PSTR("- PIC      : ")); serial_cetak_teks_ln(curr->data.pic);

            if (command == CMD_DELETE) {
                serial_cetak_teks_flash(PSTR("Hapus barang ini? (y/n): "));
                readSerialString(buffer, err);
                if (*err != ERR_OK) return;
                serial_bersihkan();

                if (buffer[0] == 'y' || buffer[0] == 'Y') {
                    InventoryNode *nextNode = curr->next;
                    deleteInventoryNode(l, curr, prev);
                    serial_cetak_teks_ln_flash(PSTR("BARANG BERHASIL DIHAPUS"));

                    serial_cetak_teks_flash(PSTR("Lanjut hapus data lain dengan nama ini? (y/n): "));
                    readSerialString(buffer, err);
                    if (buffer[0] == 'n' || buffer[0] == 'N') return;

                    curr = nextNode;
                    continue;
                }
            }
            else if (command == CMD_UPDATE_ITEM) {
                serial_cetak_teks_flash(PSTR("Update barang ini? (y/n): "));
                readSerialString(buffer, err);
                if (*err != ERR_OK) return;
                serial_bersihkan();

                if (buffer[0] == 'y' || buffer[0] == 'Y') {
                    modifyItemInventoryNode(curr, err);

                    serial_cetak_teks_flash(PSTR("Update barang lain dengan nama ini? (y/n): "));
                    readSerialString(buffer, err);
                    if (buffer[0] == 'n' || buffer[0] == 'N') return;
                }
            }
            else if (command == CMD_UPDATE_STOCK) {
                serial_cetak_teks_flash(PSTR("Update stok barang ini? (y/n): "));
                readSerialString(buffer, err);
                if (*err != ERR_OK) return;
                serial_bersihkan();

                if (buffer[0] == 'y' || buffer[0] == 'Y') {
                    modifyStockInventoryNode(curr, err);

                    serial_cetak_teks_flash(PSTR("Update stok barang lain dengan nama ini? (y/n): "));
                    readSerialString(buffer, err);
                    if (buffer[0] == 'n' || buffer[0] == 'N') return;
                }
            }
        }
        prev = curr;
        curr = curr->next;
    }

    if (foundAny) {
        serial_cetak_teks_ln_flash(PSTR("\nPencarian selesai. Semua data diperiksa."));
    } else {
        serial_cetak_teks_ln_flash(PSTR("\nBarang dengan nama tersebut tidak ditemukan."));
    }
    *err = ERR_OK;
}

void modifyItemInventoryNode(InventoryNode *curr, ErrorCode *err) {
    serial_cetak_teks_ln_flash(PSTR("\n--- Detail Data Saat Ini ---"));
    serial_cetak_teks_flash(PSTR("ID         : ")); serial_cetak_angka_ln(curr->data.itemId);
    serial_cetak_teks_flash(PSTR("1. Nama    : ")); serial_cetak_teks_ln(curr->data.itemName);
    serial_cetak_teks_flash(PSTR("2. Kategori: ")); printCategory((ItemCategory)curr->data.category);
    serial_cetak_teks_flash(PSTR("3. Lokasi  : ")); printLocation((ItemLocation)curr->data.location);
    serial_cetak_teks_flash(PSTR("4. Pemilik : ")); serial_cetak_teks_ln(curr->data.owner);
    serial_cetak_teks_flash(PSTR("5. PIC     : ")); serial_cetak_teks_ln(curr->data.pic);
    serial_cetak_teks_flash(PSTR("Pilih data yang ingin diubah (1-5): "));

    char buffer[MAX_LENGTH + 1];
    readSerialString(buffer, err);
    if (*err != ERR_OK) return;

    char choice = buffer[0];
    serial_bersihkan();

    if (choice < '1' || choice > '5') {
        serial_cetak_teks_ln_flash(PSTR("Pilihan Tidak Ada"));
        return;
    }

    serial_cetak_teks_flash(PSTR("Masukkan Data Baru: "));
    readSerialString(buffer, err);
    if (*err != ERR_OK) return;

    uint8_t numericValue = 0;

    switch (choice) {
        case '1': { 
            strncpy(curr->data.itemName, buffer, MAX_LENGTH);
            curr->data.itemName[MAX_LENGTH] = '\0';
            break;
        }
        case '2': { 
            stringToInt(buffer, &numericValue, err);
            if (*err != ERR_OK) return;
            if (numericValue > 2) { 
                serial_cetak_teks_ln_flash(PSTR("Proses Gagal, Kategori tidak valid! (Pilih 0, 1, atau 2)"));
                return;
            }
            curr->data.category = numericValue;
            break;
        }
        case '3': { 
            stringToInt(buffer, &numericValue, err);
            if (*err != ERR_OK) return;
            if (numericValue > 1) {
                serial_cetak_teks_ln_flash(PSTR("Proses Gagal, Lokasi tidak valid! (Pilih 0 atau 1)"));
                return;
            }
            curr->data.location = numericValue;
            break;
        }
        case '4': { 
            strncpy(curr->data.owner, buffer, MAX_LENGTH);
            curr->data.owner[MAX_LENGTH] = '\0';
            break;
        }
        case '5': { 
            strncpy(curr->data.pic, buffer, MAX_LENGTH);
            curr->data.pic[MAX_LENGTH] = '\0';
            break;
        }
    }
    serial_cetak_teks_ln_flash(PSTR("\nDATA BARANG BERHASIL DIPERBARUI!"));
}

void modifyStockInventoryNode(InventoryNode *curr, ErrorCode *err) {
    uint8_t availableNow = 0;
    getAvailableStock(&curr->data.stock, &availableNow);

    serial_cetak_teks_ln_flash(PSTR("\n--- Detail Stok Saat Ini ---"));
    serial_cetak_teks_flash(PSTR("1. Total Stock : ")); serial_cetak_angka_ln(curr->data.stock.totalStock);
    serial_cetak_teks_flash(PSTR("2. Dipinjam    : ")); serial_cetak_angka_ln(curr->data.stock.borrowed);
    serial_cetak_teks_flash(PSTR("3. Rusak       : ")); serial_cetak_angka_ln(curr->data.stock.broken);
    serial_cetak_teks_flash(PSTR("(Tersedia)     : ")); serial_cetak_angka_ln(availableNow);
    serial_cetak_teks_flash(PSTR("Pilih tipe stok yang ingin diubah (1-3): "));

    char buffer[MAX_LENGTH + 1];
    readSerialString(buffer, err);
    if (*err != ERR_OK) return;

    char stockChoice = buffer[0];
    serial_bersihkan();

    if (stockChoice < '1' || stockChoice > '3') {
        serial_cetak_teks_ln_flash(PSTR("Pilihan Tidak Ada"));
        return;
    }

    if (stockChoice == '1') serial_cetak_teks_flash(PSTR("Masukkan Data Stock Terbaru: "));
    if (stockChoice == '2') serial_cetak_teks_flash(PSTR("Masukkan Data Dipinjam Terbaru: "));
    if (stockChoice == '3') serial_cetak_teks_flash(PSTR("Masukkan Data Rusak Terbaru: "));

    readSerialString(buffer, err);
    if (*err != ERR_OK) return;

    uint8_t newValue = 0;
    stringToInt(buffer, &newValue, err);
    if (*err != ERR_OK) return;

    StockInfo tempStock = curr->data.stock;

    switch (stockChoice) {
        case '1': {
            if (newValue > 50) {
                serial_cetak_teks_ln_flash(PSTR("\nProses Gagal: Total stok melebihi batas maksimal lab (50 unit)!"));
                *err = ERR_OK;
                return;
            }
            tempStock.totalStock = newValue; 
            break;
        }
        case '2': tempStock.borrowed  = newValue; break;
        case '3': tempStock.broken    = newValue; break;
    }

    if ((tempStock.borrowed + tempStock.broken) > tempStock.totalStock) {
        serial_cetak_teks_ln_flash(PSTR("\nProses Gagal, Jumlah (Dipinjam dan Rusak) tidak boleh melebihi Total Stock"));
        *err = ERR_OK; 
        return;
    }

    curr->data.stock = tempStock;
    serial_cetak_teks_ln_flash(PSTR("\nSTOK BERHASIL DIPERBARUI"));
}