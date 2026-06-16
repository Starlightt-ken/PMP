#include "pinjam_barang.hpp"

void pinjamBarang(InventoryList *l, ErrorCode *err) {
    *err = ERR_OK;
    if (l == NULL || l->head == NULL) {
        Serial.println(F("\nProses Gagal: Database kosong."));
        *err = ERR_DATABASE_NULL;
        return;
    }

    char buffer[MAX_LENGTH + 1];
    Serial.print(F("\nMasukkan ID Barang yang ingin dipinjam: "));
    
    readSerialString(buffer, err);
    if (*err != ERR_OK) return;

    uint8_t targetId = 0;
    stringToInt(buffer, &targetId, err);
    if (*err != ERR_OK) return;

    for (InventoryNode *curr = l->head; curr != NULL; curr = curr->next) {
        if (curr->data.itemId == targetId) {
            uint8_t stokTersedia = 0;
            getAvailableStock(&curr->data.stock, &stokTersedia);

            if (stokTersedia > 0) {
                curr->data.stock.borrowed += 1;
                Serial.print(F("\nBERHASIL! Anda meminjam 1 unit "));
                Serial.println(curr->data.itemName);
            } else {
                Serial.println(F("\nGAGAL: Stok barang habis atau sedang dipinjam semua."));
            }
            return;
        }
    }
    Serial.println(F("\nGAGAL: ID Barang tidak ditemukan di sistem."));
}