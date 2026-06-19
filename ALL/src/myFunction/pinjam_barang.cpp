#include "pinjam_barang.hpp"

void pinjamBarang(InventoryList *l, ErrorCode *err) {
    *err = ERR_OK;
    if (l == NULL || l->head == NULL) {
        Serial.println(F("\nProses Gagal: Database kosong."));
        *err = ERR_DATABASE_NULL;
        return;
    }

    // --- ATURAN PEMINJAMAN ---
    const uint8_t MAX_JENIS_BARANG = 3;
    const uint8_t MAX_UNIT_PER_BARANG = 2;
    uint8_t jenisBarangDipinjam = 0; // Penghitung jenis barang yang sudah dipinjam di sesi ini

    Serial.println(F("\n======================================="));
    Serial.println(F("         SESI PEMINJAMAN BARANG        "));
    Serial.println(F("======================================="));
    Serial.print(F("[INFO] Maksimal meminjam "));
    Serial.print(MAX_JENIS_BARANG);
    Serial.println(F(" jenis barang berbeda."));
    Serial.print(F("[INFO] Maksimal "));
    Serial.print(MAX_UNIT_PER_BARANG);
    Serial.println(F(" unit untuk setiap barang."));

    // Looping akan berjalan selama user belum pinjam 3 jenis barang
    while (jenisBarangDipinjam < MAX_JENIS_BARANG) {
        char buffer[MAX_LENGTH + 1];
        Serial.println(F("\n---------------------------------------"));
        Serial.print(F("Masukkan ID Barang (Ketik '0' untuk selesai & keluar): "));
        
        readSerialString(buffer, err);
        if (*err != ERR_OK) return;

        uint8_t targetId = 0;
        stringToInt(buffer, &targetId, err);
        if (*err != ERR_OK) return;

        // Fitur Keluar Cepat: Kalau user ngetik 0, perulangan berhenti
        if (targetId == 0) {
            Serial.println(F("\nSesi peminjaman diakhiri oleh user."));
            break; 
        }

        bool barangDitemukan = false;

        // Proses pencarian barang
        for (InventoryNode *curr = l->head; curr != NULL; curr = curr->next) {
            if (curr->data.itemId == targetId) {
                barangDitemukan = true;
                uint8_t stokTersedia = 0;
                getAvailableStock(&curr->data.stock, &stokTersedia);

                if (stokTersedia > 0) {
                    Serial.print(F("Barang ditemukan: "));
                    Serial.println(curr->data.itemName);
                    Serial.print(F("Stok tersedia saat ini: "));
                    Serial.println(stokTersedia);

                    // Menentukan batas maksimal yang bisa diketik user
                    uint8_t batasPinjam = (stokTersedia < MAX_UNIT_PER_BARANG) ? stokTersedia : MAX_UNIT_PER_BARANG;
                    
                    Serial.print(F("Masukkan jumlah pinjam (Maks "));
                    Serial.print(batasPinjam);
                    Serial.print(F("): "));
                    
                    readSerialString(buffer, err);
                    if (*err != ERR_OK) return;

                    uint8_t jumlahPinjam = 0;
                    stringToInt(buffer, &jumlahPinjam, err);
                    if (*err != ERR_OK) return;

                    // --- VALIDASI JUMLAH ---
                    if (jumlahPinjam <= 0) {
                        Serial.println(F("\nGAGAL: Jumlah pinjam tidak valid."));
                    } else if (jumlahPinjam > MAX_UNIT_PER_BARANG) {
                        Serial.print(F("\nGAGAL: Batas maksimal hanya "));
                        Serial.print(MAX_UNIT_PER_BARANG);
                        Serial.println(F(" unit per barang!"));
                    } else if (jumlahPinjam > stokTersedia) {
                        Serial.println(F("\nGAGAL: Meminjam melebihi sisa stok yang ada!"));
                    } else {
                        // Jika semua valid, tambahkan ke data pinjaman
                        curr->data.stock.borrowed += jumlahPinjam;
                        jenisBarangDipinjam++; // Tambah kuota jenis barang
                        
                        Serial.print(F("\nBERHASIL! Anda meminjam "));
                        Serial.print(jumlahPinjam);
                        Serial.print(F(" buah "));
                        Serial.println(curr->data.itemName);
                        
                        Serial.print(F("-> Kuota jenis barang terpakai: "));
                        Serial.print(jenisBarangDipinjam);
                        Serial.print(F(" dari "));
                        Serial.println(MAX_JENIS_BARANG);
                    }
                } else {
                    Serial.println(F("\nGAGAL: Maaf, stok barang ini sedang kosong / dipinjam semua."));
                }
                break; // Keluar dari loop pencarian barang, lanjut ke input berikutnya
            }
        }

        // Kalau ID diketik sembarangan dan nggak ada di database
        if (!barangDitemukan) {
            Serial.println(F("\nGAGAL: ID Barang tersebut tidak ditemukan di sistem."));
        }

        // Peringatan jika kuota sudah habis
        if (jenisBarangDipinjam >= MAX_JENIS_BARANG) {
            Serial.println(F("\n[INFO] Kuota maksimal (3 jenis barang) sudah habis. Sesi peminjaman otomatis selesai."));
        }
    }
    
    Serial.println(F("=== KEMBALI KE MENU UTAMA ==="));
}