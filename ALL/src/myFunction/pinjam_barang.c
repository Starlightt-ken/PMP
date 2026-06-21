#include "pinjam_barang.h"
#include "serial_bridge.h"
#include <stdbool.h>

void pinjamBarang(InventoryList *l, ErrorCode *err) {
    *err = ERR_OK;
    if (l == NULL || l->head == NULL) {
        serial_cetak_teks_ln_flash(PSTR("\nProses Gagal: Database kosong."));
        *err = ERR_DATABASE_NULL;
        return;
    }

    const uint8_t MAX_JENIS_BARANG = 3;
    const uint8_t MAX_UNIT_PER_BARANG = 2;
    uint8_t jenisBarangDipinjam = 0; 

    serial_cetak_teks_ln_flash(PSTR("\n======================================="));
    serial_cetak_teks_ln_flash(PSTR("         SESI PEMINJAMAN BARANG        "));
    serial_cetak_teks_ln_flash(PSTR("======================================="));
    serial_cetak_teks_flash(PSTR("[INFO] Maksimal meminjam "));
    serial_cetak_angka(MAX_JENIS_BARANG);
    serial_cetak_teks_ln_flash(PSTR(" jenis barang berbeda."));
    serial_cetak_teks_flash(PSTR("[INFO] Maksimal "));
    serial_cetak_angka(MAX_UNIT_PER_BARANG);
    serial_cetak_teks_ln_flash(PSTR(" unit untuk setiap barang."));
    //barang yang dipinjam user maks 3 jenis dengan 2 stok per jenis
    while (jenisBarangDipinjam < MAX_JENIS_BARANG) {
        char buffer[MAX_LENGTH + 1];
        serial_cetak_teks_ln_flash(PSTR("\n---------------------------------------"));
        serial_cetak_teks_flash(PSTR("Masukkan ID Barang (Ketik '0' untuk selesai & keluar): "));
        
        readSerialString(buffer, err);
        if (*err != ERR_OK) return;

        uint8_t targetId = 0;
        stringToInt(buffer, &targetId, err);
        if (*err != ERR_OK) return;

        if (targetId == 0) {
            serial_cetak_teks_ln_flash(PSTR("\nSesi peminjaman diakhiri oleh user."));
            break; 
        }

        bool barangDitemukan = false;
        //proses pencarian barang di linked list 
        for (InventoryNode *curr = l->head; curr != NULL; curr = curr->next) {
            if (curr->data.itemId == targetId) {
                barangDitemukan = true;
                uint8_t stokTersedia = 0;
                getAvailableStock(&curr->data.stock, &stokTersedia);//menghitung stok yang tersedia (total - dipinjam - rusak)

                if (stokTersedia > 0) {
                    serial_cetak_teks_flash(PSTR("Barang ditemukan: "));
                    serial_cetak_teks_ln(curr->data.itemName);
                    serial_cetak_teks_flash(PSTR("Stok tersedia saat ini: "));
                    serial_cetak_angka_ln(stokTersedia);
                    //ternary operation : jika stok di lab kurang dari 2, maka batas pinjam mengikuti sisa stok, tetapi jika stok di lab banyak batas pinjam 2
                    uint8_t batasPinjam = (stokTersedia < MAX_UNIT_PER_BARANG) ? stokTersedia : MAX_UNIT_PER_BARANG;
                    
                    serial_cetak_teks_flash(PSTR("Masukkan jumlah pinjam (Maks "));
                    serial_cetak_angka(batasPinjam);
                    serial_cetak_teks_flash(PSTR("): "));
                    
                    readSerialString(buffer, err);
                    if (*err != ERR_OK) return;

                    uint8_t jumlahPinjam = 0;
                    stringToInt(buffer, &jumlahPinjam, err);
                    if (*err != ERR_OK) return;
                    //validasi input peminjaman berlapis
                    if (jumlahPinjam <= 0) {
                        serial_cetak_teks_ln_flash(PSTR("\nGAGAL: Jumlah pinjam tidak valid."));
                    } else if (jumlahPinjam > MAX_UNIT_PER_BARANG) {
                        serial_cetak_teks_flash(PSTR("\nGAGAL: Batas maksimal hanya "));
                        serial_cetak_angka(MAX_UNIT_PER_BARANG);
                        serial_cetak_teks_ln_flash(PSTR(" unit per barang!"));
                    } else if (jumlahPinjam > stokTersedia) {
                        serial_cetak_teks_ln_flash(PSTR("\nGAGAL: Meminjam melebihi sisa stok yang ada!"));
                    } else {
                        curr->data.stock.borrowed += jumlahPinjam; //menambahkan catatan ke sistem
                        jenisBarangDipinjam++; //menambahkan kuota barang user
                        
                        serial_cetak_teks_flash(PSTR("\nBERHASIL! Anda meminjam "));
                        serial_cetak_angka(jumlahPinjam);
                        serial_cetak_teks_flash(PSTR(" buah "));
                        serial_cetak_teks_ln(curr->data.itemName);
                        
                        serial_cetak_teks_flash(PSTR("-> Kuota jenis barang terpakai: "));
                        serial_cetak_angka(jenisBarangDipinjam);
                        serial_cetak_teks_flash(PSTR(" dari "));
                        serial_cetak_angka_ln(MAX_JENIS_BARANG);
                    }
                } else {
                    serial_cetak_teks_ln_flash(PSTR("\nGAGAL: Maaf, stok barang ini sedang kosong / dipinjam semua."));
                }
                break; 
            }
        }

        if (!barangDitemukan) {
            serial_cetak_teks_ln_flash(PSTR("\nGAGAL: ID Barang tersebut tidak ditemukan di sistem."));
        }
        //jika user sudah meminjam 3 jenis barang yang berbeda
        if (jenisBarangDipinjam >= MAX_JENIS_BARANG) {
            serial_cetak_teks_ln_flash(PSTR("\n[INFO] Kuota maksimal (3 jenis barang) sudah habis. Sesi peminjaman otomatis selesai."));
        }
    }
    
    serial_cetak_teks_ln_flash(PSTR("=== KEMBALI KE MENU UTAMA ==="));
}