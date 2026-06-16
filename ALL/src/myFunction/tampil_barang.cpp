#include "tampil_barang.hpp"

void tampilkanBarangTersedia(const InventoryList *l, ErrorCode *err) {
    *err = ERR_OK;
    if (l == NULL) {
        *err = ERR_DATABASE_NULL;
        return;
    }
    if (l->head == NULL) {
        Serial.println(F("\nDatabase kosong. Belum ada barang di laboratorium."));
        return;
    }

    Serial.println(F("\n=== DAFTAR BARANG TERSEDIA ==="));
    bool adaBarang = false;

    for (InventoryNode *curr = l->head; curr != NULL; curr = curr->next) {
        uint8_t stokTersedia = 0;
        getAvailableStock(&curr->data.stock, &stokTersedia);

        if (stokTersedia > 0) {
            adaBarang = true;
            Serial.print(F("- ID: ")); Serial.print(curr->data.itemId);
            Serial.print(F(" | Kategori: ")); printCategory((ItemCategory)curr->data.category);
            Serial.print(F(" | Nama: ")); Serial.print(curr->data.itemName);
            Serial.print(F(" | Sisa Stok: ")); Serial.println(stokTersedia);
        }
    }

    if (!adaBarang) {
        Serial.println(F("Maaf, saat ini semua barang sedang dipinjam atau rusak."));
    }
}