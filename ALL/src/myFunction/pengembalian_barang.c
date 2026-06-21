#include "pengembalian_barang.h"
#include "helper.h"
#include "serial_bridge.h"

void kembalikanBarang(InventoryList *l, ErrorCode *err) {
    *err = ERR_OK;
    if (l == NULL || l->head == NULL) { //proses validasi awal untuk memastikan apakah gudang kosong atau tidak 
        serial_cetak_teks_ln_flash(PSTR("\nProses Gagal: Database kosong."));
        *err = ERR_DATABASE_NULL;
        return;
    }

    serial_cetak_teks_ln_flash(PSTR("\n======================================="));
    serial_cetak_teks_ln_flash(PSTR("       SESI PENGEMBALIAN BARANG        "));
    serial_cetak_teks_ln_flash(PSTR("======================================="));
    //looping hingga ID 0
    while (true) {
        char buffer[MAX_LENGTH + 1];
        serial_cetak_teks_ln_flash(PSTR("\n---------------------------------------"));
        serial_cetak_teks_flash(PSTR("Masukkan ID Barang (Ketik '0' untuk selesai & keluar): "));
        
        readSerialString(buffer, err);
        if (*err != ERR_OK) return;

        uint8_t targetId = 0;
        stringToInt(buffer, &targetId, err);
        if (*err != ERR_OK) return;
        //pengakhiran fungsi 
        if (targetId == 0) {
            serial_cetak_teks_ln_flash(PSTR("\nSesi pengembalian diakhiri oleh user."));
            break; 
        }

        bool barangDitemukan = false;
        //traversal pencarian barang dengan node dummy 
        for (InventoryNode *curr = l->head; curr != NULL; curr = curr->next) {
            if (curr->data.itemId == targetId) {
                barangDitemukan = true; //barang ditemukan tanpa ada yang dipinjam terlebih dahulu 
                
                if (curr->data.stock.borrowed > 0) { //cek apakah barang sedang dipinjam 
                    serial_cetak_teks_flash(PSTR("Barang ditemukan: "));
                    serial_cetak_teks_ln(curr->data.itemName);
                    serial_cetak_teks_flash(PSTR("Catatan sistem: Ada "));
                    serial_cetak_angka(curr->data.stock.borrowed);
                    serial_cetak_teks_ln_flash(PSTR(" unit yang sedang dipinjam."));

                    serial_cetak_teks_flash(PSTR("Masukkan jumlah yang ingin dikembalikan: "));
                    readSerialString(buffer, err);
                    if (*err != ERR_OK) return;

                    uint8_t jumlahKembali = 0;
                    stringToInt(buffer, &jumlahKembali, err);
                    if (*err != ERR_OK) return;
                    //cek apakah jumlah yang dikembalikan sama dengan jumlah yang dipinjam 
                    if (jumlahKembali <= 0) {
                        serial_cetak_teks_ln_flash(PSTR("\nGAGAL: Jumlah pengembalian tidak valid."));
                    } else if (jumlahKembali > curr->data.stock.borrowed) {
                        serial_cetak_teks_ln_flash(PSTR("\nGAGAL: Anda tidak bisa mengembalikan lebih dari jumlah yang dipinjam!"));
                    } else {
                        serial_cetak_teks_flash(PSTR("Dari "));
                        serial_cetak_angka(jumlahKembali);
                        serial_cetak_teks_ln_flash(PSTR(" unit tersebut, ada berapa yang KONDISINYA RUSAK?"));
                        serial_cetak_teks_flash(PSTR("(Ketik '0' jika semuanya normal/baik): "));
                        
                        readSerialString(buffer, err);
                        if (*err != ERR_OK) return;
                        //memasukan input jumlah barang yang rusak 
                        uint8_t jumlahRusak = 0;
                        stringToInt(buffer, &jumlahRusak, err);
                        if (*err != ERR_OK) return;

                        if (jumlahRusak > jumlahKembali) {
                            serial_cetak_teks_ln_flash(PSTR("\nGAGAL: Jumlah barang rusak tidak logis (melebihi jumlah yang dikembalikan)!"));
                        } else {
                            curr->data.stock.borrowed -= jumlahKembali; 
                            curr->data.stock.broken += jumlahRusak;     
                            
                            serial_cetak_teks_ln_flash(PSTR("\n--- NOTA PENGEMBALIAN ---"));
                            serial_cetak_teks_flash(PSTR("Status: BERHASIL mengembalikan "));
                            serial_cetak_angka(jumlahKembali);
                            serial_cetak_teks_flash(PSTR(" unit "));
                            serial_cetak_teks_ln(curr->data.itemName);
                            
                            if (jumlahRusak > 0) {
                                serial_cetak_teks_flash(PSTR("Peringatan: "));
                                serial_cetak_angka(jumlahRusak);
                                serial_cetak_teks_ln_flash(PSTR(" unit dilaporkan RUSAK dan telah dikurangi dari stok tersedia!"));
                            } else {
                                serial_cetak_teks_ln_flash(PSTR("Kondisi: Semua unit dikembalikan dalam keadaan BAIK."));
                            }
                        }
                    }
                } else {
                    serial_cetak_teks_ln_flash(PSTR("\nGAGAL: Sistem mencatat semua unit barang ini sudah ada di lab (tidak ada yang sedang dipinjam)."));
                }
                break; 
            }
        }

        if (!barangDitemukan) {
            serial_cetak_teks_ln_flash(PSTR("\nGAGAL: ID Barang tidak ditemukan di sistem."));
        }
    }
    
    serial_cetak_teks_ln_flash(PSTR("=== KEMBALI KE MENU UTAMA ==="));
}