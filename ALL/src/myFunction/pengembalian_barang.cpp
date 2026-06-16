#include "pengembalian_barang.hpp"

void kembalikanBarang(InventoryList *l, ErrorCode *err) {
    *err = ERR_OK;
    if (l == NULL || l->head == NULL) {
        Serial.println(F("\nProses Gagal: Database kosong."));
        *err = ERR_DATABASE_NULL;
        return;
    }

    char buffer[MAX_LENGTH + 1];
    Serial.print(F("\nMasukkan ID Barang yang ingin dikembalikan: "));
    
    readSerialString(buffer, err);
    if (*err != ERR_OK) return;

    uint8_t targetId = 0;
    stringToInt(buffer, &targetId, err);
    if (*err != ERR_OK) return;

    for (InventoryNode *curr = l->head; curr != NULL; curr = curr->next) {
        if (curr->data.itemId == targetId) {
            if (curr->data.stock.borrowed > 0) {
                curr->data.stock.borrowed -= 1;
                Serial.print(F("\nBERHASIL! Anda mengembalikan 1 unit "));
                Serial.println(curr->data.itemName);
            } else {
                Serial.println(F("\nGAGAL: Sistem mencatat barang ini tidak sedang dipinjam."));
            }
            return;
        }
    }
    Serial.println(F("\nGAGAL: ID Barang tidak ditemukan di sistem."));
}