#include "admin_menu.h"
#include "linkedlist_core.h"
#include "serial_utils.h"
#include "helper.h"
#include "tampil_barang.h" 
#include "serial_bridge.h" 
#include "user.h"          
#include <stdio.h>        
#include <string.h>

extern User daftarUser[];
extern int indexUserAktif;
extern int totalUser; 

void tambahBarangAdmin(InventoryList *l, ErrorCode *err) {
    *err = ERR_OK;
    serial_cetak_teks_ln_flash(PSTR("\n=== SESI TAMBAH BARANG BARU ==="));
    serial_cetak_teks_ln_flash(PSTR("[INFO] Ketik ID '0' untuk selesai dan kembali ke menu."));
    
    while (true) {
        char buffer[MAX_LENGTH + 1];
        uint8_t itemId, totalStock, categoryId, locationId; 

        //input dan validasi ID
        serial_cetak_teks_flash(PSTR("\nMasukkan ID Barang Baru: "));
        readSerialString(buffer, err);
        if (*err != ERR_OK) return;
        
        stringToInt(buffer, &itemId, err); //mengubah input menjadi angka untuk dicocokan 
        if (*err != ERR_OK) {
            serial_cetak_teks_ln_flash(PSTR("GAGAL: Format ID tidak valid! Harap masukkan angka yang benar."));
            *err = ERR_OK; 
            continue; //mengulang loop dari awal 
        }
        //input yang membuat admin keluar
        if (itemId == 0) {
            serial_cetak_teks_ln_flash(PSTR("Sesi tambah barang diakhiri."));
            break; 
        }
        //mengecek apakah ada duplikasi dari ID 
        bool isExist = false;
        isItemIdExist(l, itemId, &isExist, err); //melihat keseluruhan invetoris sesuai dengan ID yang ada
        if (isExist) {
            serial_cetak_teks_ln_flash(PSTR("GAGAL: ID tersebut sudah terdaftar di gudang!"));
            serial_cetak_teks_ln_flash(PSTR("Gunakan menu PIC jika hanya ingin menambah angka stoknya."));
            continue; //mengembalikan program 
        }
        //input keterangan dari barang 
        serial_cetak_teks_flash(PSTR("Masukkan Nama Barang: "));
        char itemName[MAX_LENGTH + 1];
        readSerialString(itemName, err);

        serial_cetak_teks_ln_flash(PSTR("\nDaftar Kategori")); //memilih kategori
        for (uint8_t i = 0; i < KATEGORI_COUNT; i++) {
            serial_cetak_angka(i);
            serial_cetak_teks_flash(PSTR(". "));
            printCategory((ItemCategory)i);
        }
        serial_cetak_teks_flash(PSTR("Pilih kategori : "));
        readSerialString(buffer, err);
        stringToInt(buffer, &categoryId, err);
        
        if (*err != ERR_OK || categoryId >= KATEGORI_COUNT) {
            serial_cetak_teks_ln_flash(PSTR("GAGAL: Pilihan Kategori Tidak Ada. Ulangi input."));
            *err = ERR_OK;
            continue;
        }

        serial_cetak_teks_ln_flash(PSTR("\nDaftar Lokasi")); //memilih daftar lokasi 
        for (uint8_t i = 0; i < LOKASI_COUNT; i++) {
            serial_cetak_angka(i);
            serial_cetak_teks_flash(PSTR(". "));
            printLocation((ItemLocation)i);
        }
        serial_cetak_teks_flash(PSTR("Pilih lokasi : "));
        readSerialString(buffer, err);
        stringToInt(buffer, &locationId, err);
        
        if (*err != ERR_OK || locationId >= LOKASI_COUNT) {
            serial_cetak_teks_ln_flash(PSTR("GAGAL: Pilihan Lokasi Tidak Ada. Ulangi input."));
            *err = ERR_OK; 
            continue;
        }

        serial_cetak_teks_flash(PSTR("\nMasukkan Jumlah Stok: ")); //memilih jumlah stok 
        readSerialString(buffer, err);
        stringToInt(buffer, &totalStock, err);
        //alokasi memori untuk barang baru (menambah ruang kosong )
        InventoryNode *newNode = (InventoryNode *)malloc(sizeof(InventoryNode));
        if (newNode == NULL) {
            serial_cetak_teks_ln_flash(PSTR("\nGAGAL: Memori Arduino penuh!"));
            break;
        }
        //pengisian data 
        newNode->data.itemId = itemId;
        strncpy(newNode->data.itemName, itemName, MAX_LENGTH);
        newNode->data.category = categoryId;
        newNode->data.location = locationId; 
        
        newNode->data.stock.totalStock = totalStock;
        newNode->data.stock.borrowed = 0; 
        newNode->data.stock.broken = 0; 
        
        newNode->data.ownerIndex = indexUserAktif;
        
        serial_cetak_teks_ln_flash(PSTR("\n--- Daftar PIC Tersedia ---"));
        int picIndices[MAX_USERS]; 
        int picCount = 0;

        for (int i = 0; i < totalUser; i++) {
            if (daftarUser[i].role == ROLE_PIC) { 
                picIndices[picCount] = i; 
                picCount++;
                
                char numStr[10];
                sprintf(numStr, "%d. ", picCount);
                serial_cetak_teks(numStr);
                serial_cetak_teks_ln(daftarUser[i].username);
            }
        }

        if (picCount == 0) {
            serial_cetak_teks_ln_flash(PSTR("[INFO] Belum ada akun PIC. PIC di-set otomatis ke Tidak Diketahui."));
            newNode->data.picIndex = -1; 
        } else {
            serial_cetak_teks_flash(PSTR("Pilih nomor PIC untuk barang ini: "));
            readSerialString(buffer, err);
            
            uint8_t picChoice = 0;
            stringToInt(buffer, &picChoice, err);
            
            if (picChoice >= 1 && picChoice <= picCount) {
                int realIndex = picIndices[picChoice - 1]; 
                newNode->data.picIndex = realIndex; 
            } else {
                serial_cetak_teks_ln_flash(PSTR("[!] Pilihan salah! PIC otomatis di-set ke Tidak Diketahui."));
                newNode->data.picIndex = -1;
            }
        }

        newNode->next = NULL;

        insertNodeToList(l, newNode, err);
        
        if (*err == ERR_OK) {
            serial_cetak_teks_flash(PSTR("\nBERHASIL! Barang '"));
            serial_cetak_teks(itemName);
            serial_cetak_teks_ln_flash(PSTR("' resmi ditambahkan ke gudang."));
        } else {
            serial_cetak_teks_ln_flash(PSTR("\nGAGAL: Gudang sudah penuh (Maksimal Kapasitas)."));
            free(newNode); 
            break;
        }
    }
}

void tarikBarangAdmin(InventoryList *l, ErrorCode *err) {
    *err = ERR_OK;
    //cek apakah inventrotis kosong, kalau kosong maka proses dibatalkan
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
        //node untuk traversal 
        InventoryNode *curr = l->head;
        InventoryNode *prev = NULL;
        bool barangDitemukan = false;

        while (curr != NULL) {
            if (curr->data.itemId == targetId) { //jika ID barang sudah langsung ditemukan
                barangDitemukan = true;
                //apakah admin yang login adalah pemilik barang ini? 
                if (curr->data.ownerIndex != indexUserAktif) {
                    serial_cetak_teks_flash(PSTR("GAGAL: Anda bukan Admin pemilik barang '"));
                    serial_cetak_teks(curr->data.itemName);
                    serial_cetak_teks_ln_flash(PSTR("'. Akses ditolak!"));
                } 
                else if (curr->data.stock.borrowed > 0) { //apakah barangnya sedang dipinjam
                    serial_cetak_teks_flash(PSTR("GAGAL: Barang '"));
                    serial_cetak_teks(curr->data.itemName);
                    serial_cetak_teks_ln_flash(PSTR("' masih ada yang dipinjam. Tidak bisa ditarik!"));
                } 
                else { 
                    serial_cetak_teks_flash(PSTR("BERHASIL! Barang '"));
                    serial_cetak_teks(curr->data.itemName);
                    serial_cetak_teks_ln_flash(PSTR("' ditarik dan dihapus permanen."));
                    
                    deleteInventoryNode(l, curr, prev);
                }
                break; 
            }
            prev = curr;
            curr = curr->next;//pindah node
        }

        if (!barangDitemukan) { //barang tidak ditemukan
            serial_cetak_teks_ln_flash(PSTR("GAGAL: ID Barang tersebut tidak ditemukan di gudang."));
        }
        
        if (l->head == NULL) {
            serial_cetak_teks_ln_flash(PSTR("\n[INFO] Gudang sekarang sudah kosong sepenuhnya. Sesi otomatis diakhiri."));
            break;
        }
    }
}

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
                displayList(l, &err);
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