#include "pengembalian_barang.hpp"
#include "helper.hpp"

void kembalikanBarang(InventoryList *l, ErrorCode *err) {
    *err = ERR_OK;
    if (l == NULL || l->head == NULL) {
        Serial.println(F("\nProses Gagal: Database kosong."));
        *err = ERR_DATABASE_NULL;
        return;
    }

    Serial.println(F("\n======================================="));
    Serial.println(F("       SESI PENGEMBALIAN BARANG        "));
    Serial.println(F("======================================="));

    // Looping agar bisa mengembalikan beberapa barang sekaligus
    while (true) {
        char buffer[MAX_LENGTH + 1];
        Serial.println(F("\n---------------------------------------"));
        Serial.print(F("Masukkan ID Barang (Ketik '0' untuk selesai & keluar): "));
        
        readSerialString(buffer, err);
        if (*err != ERR_OK) return;

        uint8_t targetId = 0;
        stringToInt(buffer, &targetId, err);
        if (*err != ERR_OK) return;

        // Keluar dari perulangan jika ngetik 0
        if (targetId == 0) {
            Serial.println(F("\nSesi pengembalian diakhiri oleh user."));
            break; 
        }

        bool barangDitemukan = false;

        for (InventoryNode *curr = l->head; curr != NULL; curr = curr->next) {
            if (curr->data.itemId == targetId) {
                barangDitemukan = true;
                
                // Cek apakah barang ini memang sedang dipinjam
                if (curr->data.stock.borrowed > 0) {
                    Serial.print(F("Barang ditemukan: "));
                    Serial.println(curr->data.itemName);
                    Serial.print(F("Catatan sistem: Ada "));
                    Serial.print(curr->data.stock.borrowed);
                    Serial.println(F(" unit yang sedang dipinjam."));

                    // 1. Tanya Total Pengembalian
                    Serial.print(F("Masukkan jumlah yang ingin dikembalikan: "));
                    readSerialString(buffer, err);
                    if (*err != ERR_OK) return;

                    uint8_t jumlahKembali = 0;
                    stringToInt(buffer, &jumlahKembali, err);
                    if (*err != ERR_OK) return;

                    // Validasi input jumlah pengembalian
                    if (jumlahKembali <= 0) {
                        Serial.println(F("\nGAGAL: Jumlah pengembalian tidak valid."));
                    } else if (jumlahKembali > curr->data.stock.borrowed) {
                        Serial.println(F("\nGAGAL: Anda tidak bisa mengembalikan lebih dari jumlah yang dipinjam!"));
                    } else {
                        // 2. Tanya Kondisi Barang (Fitur Baru)
                        Serial.print(F("Dari "));
                        Serial.print(jumlahKembali);
                        Serial.println(F(" unit tersebut, ada berapa yang KONDISINYA RUSAK?"));
                        Serial.print(F("(Ketik '0' jika semuanya normal/baik): "));
                        
                        readSerialString(buffer, err);
                        if (*err != ERR_OK) return;

                        uint8_t jumlahRusak = 0;
                        stringToInt(buffer, &jumlahRusak, err);
                        if (*err != ERR_OK) return;

                        // Validasi jumlah rusak
                        if (jumlahRusak > jumlahKembali) {
                            Serial.println(F("\nGAGAL: Jumlah barang rusak tidak logis (melebihi jumlah yang dikembalikan)!"));
                        } else {
                            // 3. Eksekusi Perubahan Data
                            curr->data.stock.borrowed -= jumlahKembali; // Kurangi antrean pinjaman
                            curr->data.stock.broken += jumlahRusak;     // Tambah daftar barang rusak
                            
                            Serial.println(F("\n--- NOTA PENGEMBALIAN ---"));
                            Serial.print(F("Status: BERHASIL mengembalikan "));
                            Serial.print(jumlahKembali);
                            Serial.print(F(" unit "));
                            Serial.println(curr->data.itemName);
                            
                            // Berikan notifikasi khusus jika ada yang rusak
                            if (jumlahRusak > 0) {
                                Serial.print(F("Peringatan: "));
                                Serial.print(jumlahRusak);
                                Serial.println(F(" unit dilaporkan RUSAK dan telah dikurangi dari stok tersedia!"));
                            } else {
                                Serial.println(F("Kondisi: Semua unit dikembalikan dalam keadaan BAIK."));
                            }
                        }
                    }
                } else {
                    Serial.println(F("\nGAGAL: Sistem mencatat semua unit barang ini sudah ada di lab (tidak ada yang sedang dipinjam)."));
                }
                break; // Keluar dari loop pencarian, lanjut ke barang berikutnya
            }
        }

        if (!barangDitemukan) {
            Serial.println(F("\nGAGAL: ID Barang tidak ditemukan di sistem."));
        }
    }
    
    Serial.println(F("=== KEMBALI KE MENU UTAMA ==="));
}