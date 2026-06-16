#include "inventory_operation.hpp"

void destroyList(InventoryList *l, ErrorCode *err) {

    destroyListCore(l, err);

    if (*err == ERR_DATABASE_NULL) {
        return;
    }
    else if (*err == ERR_LIST_EMPTY) {
        Serial.println(F("\nDatabase kosong, tidak ada data untuk dihapus"));
    } 
    else {
        Serial.println(F("Semua data berhasil dihapus"));
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
        Serial.println(F("Gagal alokasi node"));
        return;
    }

    memset(newNode, 0, sizeof(InventoryNode));
    newNode->next = NULL;

    // INPUT
    getInput(l, newNode, err);

    if (*err != ERR_OK) {
        free(newNode);
        return;
    }

    // INSERT
    insertNodeToList(l, newNode, err);

    if (*err != ERR_OK) {
        free(newNode);
        return;
    }

    // OUTPUT
    Serial.println(F("\n-- Rangkuman Data Baru --"));
    displayItemData(&newNode->data);
    Serial.println(F("BERHASIL DITAMBAHKAN"));
}

void runInventoryMenu(InventoryList *l, MenuCommand command, ErrorCode *err) {
    *err = ERR_OK;

    if (l == NULL) {
        *err = ERR_DATABASE_NULL;
        return;
    }

    if (l->head == NULL) {
        Serial.println(F("\nDatabase kosong"));
        *err = ERR_OK;
        return;
    }

    clearSerial();

    switch (command) {
        case CMD_SEARCH: {
            Serial.println(F("\n=== CARI DATA BARANG ==="));
            break;
        }
        case CMD_DELETE: {
            Serial.println(F("\n=== HAPUS DATA BARANG ==="));
            break;
        }
        case CMD_UPDATE_ITEM: {
            Serial.println(F("\n=== UPDATE DATA BARANG ==="));
            break;
        }
        case CMD_UPDATE_STOCK: {
            Serial.println(F("\n=== UPDATE STOK BARANG ==="));
            break;
        }
    }

    Serial.println(F("1. Berdasarkan ID"));
    Serial.println(F("2. Berdasarkan Nama"));
    Serial.print(F("Pilih metode: "));

    char buffer[MAX_LENGTH + 1];

    readSerialString(buffer, err);
    if (*err != ERR_OK) return;

    char choice = buffer[0];

    clearSerial();

    if (choice == '1') {
        Serial.print(F("Masukkan ID Barang: "));

        readSerialString(buffer, err);
        if (*err != ERR_OK) return;

        uint8_t targetId = 0;

        stringToInt(buffer, &targetId, err);
        if (*err != ERR_OK) return;

        processById(l, targetId, command, err);
        if (*err != ERR_OK) return;
    }
    else if (choice == '2') {
        Serial.print(F("Masukkan Nama Barang: "));

        readSerialString(buffer, err);
        if (*err != ERR_OK) return;

        processByName(l, buffer, command, err);
        if (*err != ERR_OK) return;

    }
    else {
        Serial.println(F("Pilihan Tidak Ada"));
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
                    Serial.println(F("--- BARANG DITEMUKAN ---"));
                    displayItemData(&curr->data);
                    break;
                }
                case CMD_DELETE: {
                    deleteInventoryNode(l, curr, prev);

                    Serial.print(F("\nBarang dengan ID "));
                    Serial.print(targetId);
                    Serial.println(F(" BERHASIL DIHAPUS"));
                    break;
                }
                case CMD_UPDATE_ITEM: {
                    Serial.print(F("\nBarang Ditemukan: "));
                    Serial.println(curr->data.itemName);

                    modifyItemInventoryNode(curr, err);
                    break;
                }
                case CMD_UPDATE_STOCK: {
                    Serial.print(F("\nBarang Ditemukan: "));
                    Serial.println(curr->data.itemName);

                    modifyStockInventoryNode(curr, err);
                    break;
                }
            }
            return;
        }
        prev = curr;
        curr = curr->next;
    }
    Serial.println(F("\nBarang dengan ID tersebut tidak ditemukan."));
}

void processByName(InventoryList *l, const char *targetName, MenuCommand command, ErrorCode *err) {

    // =========================
    // SEARCH (beda sendiri)
    // =========================
    if (command == CMD_SEARCH) {
        Serial.print(F("\nMencari Nama: "));
        Serial.println(targetName);

        bool found = false;

        for (InventoryNode *curr = l->head; curr != NULL; curr = curr->next) {
            if (strcmp(curr->data.itemName, targetName) == 0) {
                if (!found) Serial.println(F("--- BARANG DITEMUKAN ---"));
                displayItemData(&curr->data);
                found = true;
            }
        }

        if (!found) {
            Serial.println(F("Barang dengan nama tersebut tidak ditemukan."));
        }

        *err = ERR_OK;
        return;
    }

    // =========================
    // DELETE / UPDATE / STOCK
    // =========================

    bool foundAny = false;
    char buffer[2];

    InventoryNode *curr = l->head;
    InventoryNode *prev = NULL;

    while (curr != NULL) {

        if (strcmp(curr->data.itemName, targetName) == 0) {
            foundAny = true;

            Serial.println(F("\nBarang Ditemukan:"));
            Serial.print(F("- ID       : ")); Serial.println(curr->data.itemId);
            Serial.print(F("- Kategori : ")); printCategory((ItemCategory)curr->data.category);
            Serial.print(F("- Lokasi   : ")); printLocation((ItemLocation)curr->data.location);
            Serial.print(F("- Pemilik  : ")); Serial.println(curr->data.owner);
            Serial.print(F("- PIC      : ")); Serial.println(curr->data.pic);

            // =========================
            // DELETE
            // =========================
            if (command == CMD_DELETE) {

                Serial.print(F("Hapus barang ini? (y/n): "));
                readSerialString(buffer, err);
                if (*err != ERR_OK) return;
                clearSerial();

                if (buffer[0] == 'y' || buffer[0] == 'Y') {

                    InventoryNode *nextNode = curr->next;

                    deleteInventoryNode(l, curr, prev);

                    Serial.println(F("BARANG BERHASIL DIHAPUS"));

                    Serial.print(F("Lanjut hapus data lain dengan nama ini? (y/n): "));
                    readSerialString(buffer, err);
                    if (buffer[0] == 'n' || buffer[0] == 'N') return;

                    curr = nextNode;
                    continue;
                }
            }

            // =========================
            // UPDATE ITEM
            // =========================
            else if (command == CMD_UPDATE_ITEM) {

                Serial.print(F("Update barang ini? (y/n): "));
                readSerialString(buffer, err);
                if (*err != ERR_OK) return;
                clearSerial();

                if (buffer[0] == 'y' || buffer[0] == 'Y') {

                    modifyItemInventoryNode(curr, err);

                    Serial.print(F("Update barang lain dengan nama ini? (y/n): "));
                    readSerialString(buffer, err);
                    if (buffer[0] == 'n' || buffer[0] == 'N') return;
                }
            }

            // =========================
            // UPDATE STOCK
            // =========================
            else if (command == CMD_UPDATE_STOCK) {

                Serial.print(F("Update stok barang ini? (y/n): "));
                readSerialString(buffer, err);
                if (*err != ERR_OK) return;
                clearSerial();

                if (buffer[0] == 'y' || buffer[0] == 'Y') {

                    modifyStockInventoryNode(curr, err);

                    Serial.print(F("Update stok barang lain dengan nama ini? (y/n): "));
                    readSerialString(buffer, err);
                    if (buffer[0] == 'n' || buffer[0] == 'N') return;
                }
            }
        }

        prev = curr;
        curr = curr->next;
    }

    if (foundAny) {
        Serial.println(F("\nPencarian selesai. Semua data diperiksa."));
    } else {
        Serial.println(F("\nBarang dengan nama tersebut tidak ditemukan."));
    }

    *err = ERR_OK;
}

void modifyItemInventoryNode(InventoryNode *curr, ErrorCode *err) {

    Serial.println(F("\n--- Detail Data Saat Ini ---"));
    Serial.print(F("ID         : ")); Serial.println(curr->data.itemId);
    Serial.print(F("1. Nama    : ")); Serial.println(curr->data.itemName);
    Serial.print(F("2. Kategori: ")); printCategory((ItemCategory)curr->data.category);
    Serial.print(F("3. Lokasi  : ")); printLocation((ItemLocation)curr->data.location);
    Serial.print(F("4. Pemilik : ")); Serial.println(curr->data.owner);
    Serial.print(F("5. PIC     : ")); Serial.println(curr->data.pic);
    Serial.print(F("Pilih data yang ingin diubah (1-5): "));

    char buffer[MAX_LENGTH + 1];
    readSerialString(buffer, err);
    if (*err != ERR_OK) return;

    char choice = buffer[0];
    clearSerial();

    if (choice < '1' || choice > '5') {
        Serial.println(F("Pilihan Tidak Ada"));
        return;
    }

    Serial.print(F("Masukkan Data Baru: "));
    readSerialString(buffer, err);
    if (*err != ERR_OK) return;

    uint8_t numericValue = 0;

    switch (choice) {
        case '1': { // Ubah Nama
            strncpy(curr->data.itemName, buffer, MAX_LENGTH);
            curr->data.itemName[MAX_LENGTH] = '\0';
            break;
        }

        case '2': { // Ubah Kategori
            stringToInt(buffer, &numericValue, err);
            if (*err != ERR_OK) return;
            if (numericValue < 1 || numericValue > 4) { 
                Serial.println(F("Proses Gagal, Kategori tidak valid!"));
                return;
            }
            curr->data.category = numericValue;
            break;
        }

        case '3': { // Ubah Lokasi
            stringToInt(buffer, &numericValue, err);
            if (*err != ERR_OK) return;
            if (numericValue < 1 || numericValue > 3) {
                Serial.println(F("Proses Gagal, Lokasi tidak valid!"));
                return;
            }
            curr->data.location = numericValue;
            break;
        }

        case '4': { // Ubah Pemilik
            strncpy(curr->data.owner, buffer, MAX_LENGTH);
            curr->data.owner[MAX_LENGTH] = '\0';
            break;
        }

        case '5': { // Ubah PIC
            strncpy(curr->data.pic, buffer, MAX_LENGTH);
            curr->data.pic[MAX_LENGTH] = '\0';
            break;
        }
    }

    Serial.println(F("\nDATA BARANG BERHASIL DIPERBARUI!"));
}

void modifyStockInventoryNode(InventoryNode *curr, ErrorCode *err) {

    uint8_t availableNow = 0;
    getAvailableStock(&curr->data.stock, &availableNow);

    Serial.println(F("\n--- Detail Stok Saat Ini ---"));
    Serial.print(F("1. Total Stock : "));
    Serial.println(curr->data.stock.totalStock);
    Serial.print(F("2. Dipinjam    : "));
    Serial.println(curr->data.stock.borrowed);
    Serial.print(F("3. Rusak       : "));
    Serial.println(curr->data.stock.broken);
    Serial.print(F("(Tersedia)     : "));
    Serial.println(availableNow);
    Serial.print(F("Pilih tipe stok yang ingin diubah (1-3): "));

    char buffer[MAX_LENGTH + 1];
    readSerialString(buffer, err);
    if (*err != ERR_OK) return;

    char stockChoice = buffer[0];
    clearSerial();

    if (stockChoice < '1' || stockChoice > '3') {
        Serial.println(F("Pilihan Tidak Ada"));
        return;
    }

    if (stockChoice == '1') Serial.print(F("Masukkan Data Stock Terbaru: "));
    if (stockChoice == '2') Serial.print(F("Masukkan Data Dipinjam Terbaru: "));
    if (stockChoice == '3') Serial.print(F("Masukkan Data Rusak Terbaru: "));

    readSerialString(buffer, err);
    if (*err != ERR_OK) return;

    uint8_t newValue = 0;
    stringToInt(buffer, &newValue, err);
    if (*err != ERR_OK) return;

    // Salin stok ke struct sementara untuk validasi kelayakan logis
    StockInfo tempStock = curr->data.stock;

    switch (stockChoice) {
        case '1': tempStock.totalStock = newValue; break;
        case '2': tempStock.borrowed  = newValue; break;
        case '3': tempStock.broken    = newValue; break;
    }

    // validasi barang melebihi stock
    if ((tempStock.borrowed + tempStock.broken) > tempStock.totalStock) {
        Serial.println(F("\nProses Gagal, Jumlah (Dipinjam dan Rusak) tidak boleh melebihi Total Stock"));
        *err = ERR_OK; 
        return;
    }

    // jika validasi benar, update data asli di heap
    curr->data.stock = tempStock;
    Serial.println(F("\nSTOK BERHASIL DIPERBARUI"));
}