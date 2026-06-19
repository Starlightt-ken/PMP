#include "admin_menu.h"
#include "linkedlist_core.h"
#include "serial_utils.h"
#include "helper.h"
#include "tampil_barang.h" 
#include "serial_bridge.h" // Wajib dipanggil untuk output

// =========================================================
// FUNGSI INTERNAL ADMIN
// =========================================================

void tambahBarangAdmin(InventoryList *l, ErrorCode *err) {
    *err = ERR_OK;
    serial_cetak_teks_ln_flash(PSTR("\n=== SESI TAMBAH BARANG BARU ==="));
    serial_cetak_teks_ln_flash(PSTR("[INFO] Ketik ID '0' untuk selesai dan kembali ke menu."));
    
    while (true) {
        char buffer[MAX_LENGTH + 1];
        uint8_t itemId, totalStock, categoryId;

        // 1. Minta ID
        serial_cetak_teks_flash(PSTR("\nMasukkan ID Barang Baru: "));
        readSerialString(buffer, err);
        if (*err != ERR_OK) return;
        stringToInt(buffer, &itemId, err);

        // Fitur Keluar
        if (itemId == 0) {
            serial_cetak_teks_ln_flash(PSTR("Sesi tambah barang diakhiri."));
            break; 
        }

        // --- CEK KEAMANAN ID GANDA ---
        bool isExist = false;
        isItemIdExist(l, itemId, &isExist, err);
        if (isExist) {
            serial_cetak_teks_ln_flash(PSTR("GAGAL: ID tersebut sudah terdaftar di gudang!"));
            serial_cetak_teks_ln_flash(PSTR("Gunakan menu PIC jika hanya ingin menambah angka stoknya."));
            continue; 
        }

        // 2. Minta Nama
        serial_cetak_teks_flash(PSTR("Masukkan Nama Barang: "));
        char itemName[MAX_LENGTH + 1];
        readSerialString(itemName, err);

        // 3. Minta Kategori
        serial_cetak_teks_flash(PSTR("Masukkan Kategori (0-2): ")); 
        readSerialString(buffer, err);
        stringToInt(buffer, &categoryId, err);

        // 4. Minta Stok
        serial_cetak_teks_flash(PSTR("Masukkan Jumlah Stok: "));
        readSerialString(buffer, err);
        stringToInt(buffer, &totalStock, err);

        // Memesan memori
        InventoryNode *newNode = (InventoryNode *)malloc(sizeof(InventoryNode));
        if (newNode == NULL) {
            serial_cetak_teks_ln_flash(PSTR("\nGAGAL: Memori Arduino penuh!"));
            break;
        }

        // Mengisi data
        newNode->data.itemId = itemId;
        strncpy(newNode->data.itemName, itemName, MAX_LENGTH);
        newNode->data.category = categoryId;
        
        newNode->data.stock.totalStock = totalStock;
        newNode->data.stock.borrowed = 0; 
        newNode->data.stock.broken = 0; 
        
        strcpy(newNode->data.owner, "Admin"); 
        strcpy(newNode->data.pic, "-");       
        
        newNode->next = NULL;

        insertNodeToList(l, newNode, err);
        
        if (*err == ERR_OK) {
            serial_cetak_teks_flash(PSTR("BERHASIL! Barang '"));
            serial_cetak_teks(itemName);
            serial_cetak_teks_ln_flash(PSTR("' resmi ditambahkan ke gudang."));
        } else {
            serial_cetak_teks_ln_flash(PSTR("GAGAL: Gudang sudah penuh (Maksimal Kapasitas)."));
            free(newNode); 
            break;
        }
    }
}

void tarikBarangAdmin(InventoryList *l, ErrorCode *err) {
    *err = ERR_OK;

    if (l == NULL || l->head == NULL) {
        serial_cetak_teks_ln_flash(PSTR("\nProses Gagal: Gudang masih kosong."));
        return;
    }

    serial_cetak_teks_ln_flash(PSTR("\n=== SESI TARIK BARANG PERMANEN ==="));
    serial_cetak_teks_ln_flash(PSTR("[INFO] Ketik ID '0' untuk selesai dan kembali ke menu."));

    while (true) {
        char buffer[MAX_LENGTH + 1];
        uint8_t targetId = 0;

        serial_cetak_teks_flash(PSTR("\nMasukkan ID Barang yang ingin ditarik: "));
        readSerialString(buffer, err);
        if (*err != ERR_OK) return;
        
        stringToInt(buffer, &targetId, err);
        if (*err != ERR_OK) return;

        if (targetId == 0) {
            serial_cetak_teks_ln_flash(PSTR("Sesi penarikan barang diakhiri."));
            break; 
        }

        InventoryNode *curr = l->head;
        InventoryNode *prev = NULL;
        bool barangDitemukan = false;

        while (curr != NULL) {
            if (curr->data.itemId == targetId) {
                barangDitemukan = true;

                if (curr->data.stock.borrowed > 0) {
                    serial_cetak_teks_flash(PSTR("GAGAL: Barang '"));
                    serial_cetak_teks(curr->data.itemName);
                    serial_cetak_teks_ln_flash(PSTR("' masih ada yang dipinjam. Tidak bisa ditarik!"));
                } else {
                    serial_cetak_teks_flash(PSTR("BERHASIL! Barang '"));
                    serial_cetak_teks(curr->data.itemName);
                    serial_cetak_teks_ln_flash(PSTR("' ditarik dan dihapus permanen."));
                    
                    deleteInventoryNode(l, curr, prev);
                }
                break; 
            }
            prev = curr;
            curr = curr->next;
        }

        if (!barangDitemukan) {
            serial_cetak_teks_ln_flash(PSTR("GAGAL: ID Barang tersebut tidak ditemukan di gudang."));
        }
        
        if (l->head == NULL) {
            serial_cetak_teks_ln_flash(PSTR("\n[INFO] Gudang sekarang sudah kosong sepenuhnya. Sesi otomatis diakhiri."));
            break;
        }
    }
}

// =========================================================
// MENU UTAMA ADMIN
// =========================================================

void menuAdmin(InventoryList *l) {
    bool diMenuAdmin = true;
    ErrorCode err = ERR_OK;
    char buffer[MAX_LENGTH + 1];

    while (diMenuAdmin) {
        serial_cetak_teks_ln_flash(PSTR("\n==============================="));
        serial_cetak_teks_ln_flash(PSTR("  MENU ADMIN (PEMILIK BARANG)  "));
        serial_cetak_teks_ln_flash(PSTR("==============================="));
        serial_cetak_teks_ln_flash(PSTR("1. Lihat Daftar Barang di Gudang"));
        serial_cetak_teks_ln_flash(PSTR("2. Tambah / Titip Barang Baru"));
        serial_cetak_teks_ln_flash(PSTR("3. Tarik Barang Permanen"));
        serial_cetak_teks_ln_flash(PSTR("4. Logout"));
        serial_cetak_teks_flash(PSTR("Pilih menu (1-4): "));

        readSerialString(buffer, &err);
        if (err != ERR_OK) continue;

        char choice = buffer[0];
        clearSerial();

        switch (choice) {
            case '1': 
                tampilkanBarangTersedia(l, &err); 
                break;
            case '2': 
                tambahBarangAdmin(l, &err);
                break;
            case '3': 
                tarikBarangAdmin(l, &err);
                break;
            case '4': 
                serial_cetak_teks_ln_flash(PSTR("\nLogout berhasil. Kembali ke menu login..."));
                diMenuAdmin = false; 
                break;
            default: 
                serial_cetak_teks_ln_flash(PSTR("\nPilihan tidak valid. Silakan coba lagi.")); 
                break;
        }
    }
}