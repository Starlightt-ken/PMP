#include "tampil_barang.h"
#include "serial_bridge.h"

void tampilkanBarangTersedia(const InventoryList *l, ErrorCode *err) {
    *err = ERR_OK;
    if (l == NULL) {
        *err = ERR_DATABASE_NULL;
        return;
    }
    if (l->head == NULL) {
        serial_cetak_teks_ln_flash(PSTR("\nDatabase kosong. Belum ada barang di laboratorium."));
        return;
    }

    serial_cetak_teks_ln_flash(PSTR("\n=== DAFTAR BARANG TERSEDIA ==="));
    bool adaBarang = false;

    for (InventoryNode *curr = l->head; curr != NULL; curr = curr->next) {
        uint8_t stokTersedia = 0;
        getAvailableStock(&curr->data.stock, &stokTersedia);

        if (stokTersedia > 0) {
            adaBarang = true;
            serial_cetak_teks_flash(PSTR("- ID: ")); 
            serial_cetak_angka(curr->data.itemId);
            serial_cetak_teks_flash(PSTR(" | Kategori: ")); 
            printCategory((ItemCategory)curr->data.category);
            serial_cetak_teks_flash(PSTR(" | Nama: ")); 
            serial_cetak_teks(curr->data.itemName);
            serial_cetak_teks_flash(PSTR(" | Sisa Stok: ")); 
            serial_cetak_angka_ln(stokTersedia);
        }
    }

    if (!adaBarang) {
        serial_cetak_teks_ln_flash(PSTR("Maaf, saat ini semua barang sedang dipinjam atau rusak."));
    }
}