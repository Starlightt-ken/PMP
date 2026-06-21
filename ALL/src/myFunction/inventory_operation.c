#include "inventory_operation.h"
#include "serial_bridge.h"
#include "serial_utils.h"
#include "helper.h"
#include "user.h"
#include <string.h>

void runInventoryMenu(InventoryList *l, MenuCommand cmd, ErrorCode *err) {
    *err = ERR_OK;
    if (l == NULL || l->head == NULL) {
        serial_cetak_teks_ln_flash(PSTR("\nGAGAL: Gudang saat ini kosong!"));
        return;
    }

    // Tampilkan Header Sesuai Perintah
    if (cmd == CMD_SEARCH) serial_cetak_teks_ln_flash(PSTR("\n=== CARI DATA BARANG ==="));
    else if (cmd == CMD_UPDATE_STOCK) serial_cetak_teks_ln_flash(PSTR("\n=== UPDATE STOK BARANG ==="));
    else if (cmd == CMD_UPDATE_ITEM) serial_cetak_teks_ln_flash(PSTR("\n=== UPDATE DATA BARANG ==="));

    serial_cetak_teks_ln_flash(PSTR("1. Berdasarkan ID"));
    serial_cetak_teks_ln_flash(PSTR("2. Berdasarkan Nama"));
    serial_cetak_teks_flash(PSTR("Pilih metode: "));

    char buffer[MAX_LENGTH + 1];
    readSerialString(buffer, err);
    if (*err != ERR_OK) return;

    char method = buffer[0];
    uint8_t targetId = 0;
    char targetName[MAX_LENGTH + 1] = {0};

    // Ambil Input Target
    if (method == '1') {
        serial_cetak_teks_flash(PSTR("Masukkan ID Barang: "));
        readSerialString(buffer, err);
        stringToInt(buffer, &targetId, err);
        if (*err != ERR_OK) {
            serial_cetak_teks_ln_flash(PSTR("GAGAL: Format ID harus berupa angka!"));
            return;
        }
    } else if (method == '2') {
        serial_cetak_teks_flash(PSTR("Masukkan Nama Barang: "));
        readSerialString(targetName, err);
        if (*err != ERR_OK) return;
    } else {
        serial_cetak_teks_ln_flash(PSTR("GAGAL: Metode pencarian tidak valid!"));
        return;
    }

    InventoryNode *curr = l->head;
    bool found = false;

    // Proses Pencarian di Linked List
    while (curr != NULL) {
        bool match = false;
        
        if (method == '1' && curr->data.itemId == targetId) match = true;
        else if (method == '2' && strcasecmp(curr->data.itemName, targetName) == 0) match = true;

        if (match) {
            found = true;
            serial_cetak_teks_ln_flash(PSTR("\n--- BARANG DITEMUKAN ---"));
            serial_cetak_teks_flash(PSTR("ID       : ")); serial_cetak_angka(curr->data.itemId); serial_cetak_teks_ln_flash(PSTR(""));
            serial_cetak_teks_flash(PSTR("Nama     : ")); serial_cetak_teks_ln(curr->data.itemName);

            // ==========================================
            // LOGIKA 1: JIKA HANYA MENCARI (SEARCH)
            // ==========================================
            if (cmd == CMD_SEARCH) {
                uint8_t tersedia = curr->data.stock.totalStock - curr->data.stock.borrowed - curr->data.stock.broken;
                serial_cetak_teks_flash(PSTR("Total    : ")); serial_cetak_angka(curr->data.stock.totalStock); serial_cetak_teks_ln_flash(PSTR(""));
                serial_cetak_teks_flash(PSTR("Tersedia : ")); serial_cetak_angka(tersedia); serial_cetak_teks_ln_flash(PSTR(""));
                serial_cetak_teks_flash(PSTR("Dipinjam : ")); serial_cetak_angka(curr->data.stock.borrowed); serial_cetak_teks_ln_flash(PSTR(""));
                serial_cetak_teks_flash(PSTR("Rusak    : ")); serial_cetak_angka(curr->data.stock.broken); serial_cetak_teks_ln_flash(PSTR(""));
                serial_cetak_teks_ln_flash(PSTR("------------------------"));
            }
            // ==========================================
            // LOGIKA 2: JIKA INGIN UPDATE STOK
            // ==========================================
            else if (cmd == CMD_UPDATE_STOCK) {
                uint8_t tersedia = curr->data.stock.totalStock - curr->data.stock.borrowed - curr->data.stock.broken;
                serial_cetak_teks_ln_flash(PSTR("\n--- Detail Stok Saat Ini ---"));
                serial_cetak_teks_flash(PSTR("1. Total Stock : ")); serial_cetak_angka(curr->data.stock.totalStock); serial_cetak_teks_ln_flash(PSTR(""));
                serial_cetak_teks_flash(PSTR("2. Dipinjam    : ")); serial_cetak_angka(curr->data.stock.borrowed); serial_cetak_teks_ln_flash(PSTR(""));
                serial_cetak_teks_flash(PSTR("3. Rusak       : ")); serial_cetak_angka(curr->data.stock.broken); serial_cetak_teks_ln_flash(PSTR(""));
                serial_cetak_teks_flash(PSTR("(Tersedia)     : ")); serial_cetak_angka(tersedia); serial_cetak_teks_ln_flash(PSTR(""));
                
                serial_cetak_teks_flash(PSTR("Pilih tipe stok yang ingin diubah (1-3): "));
                readSerialString(buffer, err);
                uint8_t tipe = buffer[0] - '0';

                if (tipe >= 1 && tipe <= 3) {
                    serial_cetak_teks_flash(PSTR("Masukkan Data Terbaru: "));
                    readSerialString(buffer, err);
                    uint8_t val = 0;
                    stringToInt(buffer, &val, err);
                    if (*err == ERR_OK) {
                        if (tipe == 1) curr->data.stock.totalStock = val;
                        else if (tipe == 2) curr->data.stock.borrowed = val;
                        else if (tipe == 3) curr->data.stock.broken = val;
                        serial_cetak_teks_ln_flash(PSTR("STOK BERHASIL DIPERBARUI!"));
                    }
                } else {
                    serial_cetak_teks_ln_flash(PSTR("GAGAL: Pilihan tipe stok tidak valid."));
                }
                break; // Keluar dari loop setelah selesai update (FIX UX BUG)
            }
            // ==========================================
            // LOGIKA 3: JIKA INGIN UPDATE IDENTITAS
            // ==========================================
            else if (cmd == CMD_UPDATE_ITEM) {
                serial_cetak_teks_ln_flash(PSTR("\n--- Detail Data Saat Ini ---"));
                serial_cetak_teks_flash(PSTR("1. Nama     : ")); serial_cetak_teks_ln(curr->data.itemName);
                serial_cetak_teks_flash(PSTR("2. Kategori : ")); serial_cetak_angka(curr->data.category); serial_cetak_teks_ln_flash(PSTR(""));
                serial_cetak_teks_flash(PSTR("3. Lokasi   : ")); serial_cetak_angka(curr->data.location); serial_cetak_teks_ln_flash(PSTR(""));
                serial_cetak_teks_flash(PSTR("4. PIC      : ")); serial_cetak_angka(curr->data.picIndex); serial_cetak_teks_ln_flash(PSTR(""));
                
                serial_cetak_teks_flash(PSTR("Pilih data yang ingin diubah (1-4): "));
                readSerialString(buffer, err);
                uint8_t tipe = buffer[0] - '0';

                if (tipe == 1) {
                    serial_cetak_teks_flash(PSTR("Masukkan Nama Baru: "));
                    readSerialString(curr->data.itemName, err);
                    serial_cetak_teks_ln_flash(PSTR("DATA BERHASIL DIPERBARUI!"));
                } else if (tipe >= 2 && tipe <= 4) {
                    serial_cetak_teks_flash(PSTR("Masukkan Angka ID Baru: "));
                    readSerialString(buffer, err);
                    uint8_t val = 0;
                    stringToInt(buffer, &val, err);
                    if (*err == ERR_OK) {
                        if (tipe == 2) curr->data.category = val;
                        else if (tipe == 3) curr->data.location = val;
                        else if (tipe == 4) curr->data.picIndex = val;
                        serial_cetak_teks_ln_flash(PSTR("DATA BERHASIL DIPERBARUI!"));
                    }
                } else {
                    serial_cetak_teks_ln_flash(PSTR("GAGAL: Pilihan data tidak valid."));
                }
                break; // Keluar dari loop setelah selesai update (FIX UX BUG)
            }
            
            // Jika pencarian via ID, langsung break (karena ID tidak mungkin kembar)
            if (method == '1') break; 
        }
        curr = curr->next;
    }

    if (!found) {
        serial_cetak_teks_ln_flash(PSTR("\nBarang tidak ditemukan di gudang."));
    } else {
        serial_cetak_teks_ln_flash(PSTR("\nSesi selesai. Kembali ke menu PIC."));
    }
}