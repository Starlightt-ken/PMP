#include "admin_menu.hpp"
#include "linkedlist_core.hpp"
#include "serial_utils.hpp"
#include "helper.hpp"
#include "tampil_barang.hpp" // Pinjam fitur tampil barang buatanmu sebelumnya

// =========================================================
// FUNGSI INTERNAL ADMIN
// =========================================================

void tambahBarangAdmin(InventoryList *l, ErrorCode *err) {
    *err = ERR_OK;
    Serial.println(F("\n=== SESI TAMBAH BARANG BARU ==="));
    Serial.println(F("[INFO] Ketik ID '0' untuk selesai dan kembali ke menu."));
    
    // Pakai Looping agar Admin bisa nambah banyak barang sekaligus tanpa bolak-balik
    while (true) {
        char buffer[MAX_LENGTH + 1];
        uint8_t itemId, totalStock, categoryId;

        // 1. Minta ID
        Serial.print(F("\nMasukkan ID Barang Baru: "));
        readSerialString(buffer, err);
        if (*err != ERR_OK) return;
        stringToInt(buffer, &itemId, err);

        // Fitur Keluar
        if (itemId == 0) {
            Serial.println(F("Sesi tambah barang diakhiri."));
            break; 
        }

        // --- CEK KEAMANAN ID GANDA ---
        bool isExist = false;
        // Kita pinjam fungsi isItemIdExist buatan temanmu dari helper.hpp
        isItemIdExist(l, itemId, &isExist, err);
        if (isExist) {
            Serial.println(F("GAGAL: ID tersebut sudah terdaftar di gudang!"));
            Serial.println(F("Gunakan menu PIC jika hanya ingin menambah angka stoknya."));
            continue; // Langsung lompat ke perulangan awal minta ID lagi
        }

        // 2. Minta Nama
        Serial.print(F("Masukkan Nama Barang: "));
        char itemName[MAX_LENGTH + 1];
        readSerialString(itemName, err);

        // 3. Minta Kategori
        // Berdasarkan types.hpp: 0=Sensor, 1=Aktuator, 2=Mikrokontroller
        Serial.print(F("Masukkan Kategori (0-2): ")); 
        readSerialString(buffer, err);
        stringToInt(buffer, &categoryId, err);

        // 4. Minta Stok
        Serial.print(F("Masukkan Jumlah Stok: "));
        readSerialString(buffer, err);
        stringToInt(buffer, &totalStock, err);

        // Memesan memori untuk kantong (Node) baru
        InventoryNode *newNode = (InventoryNode *)malloc(sizeof(InventoryNode));
        if (newNode == NULL) {
            Serial.println(F("\nGAGAL: Memori Arduino penuh!"));
            break;
        }

        // Mengisi data ke dalam kantong
        newNode->data.itemId = itemId;
        strncpy(newNode->data.itemName, itemName, MAX_LENGTH);
        newNode->data.category = categoryId;
        
        newNode->data.stock.totalStock = totalStock;
        newNode->data.stock.borrowed = 0; 
        newNode->data.stock.broken = 0; // Inisialisasi wajib agar datanya tidak ngacak
        
        // Kasih nama default untuk mencegah error memori
        strcpy(newNode->data.owner, "Admin"); 
        strcpy(newNode->data.pic, "-");       
        
        newNode->next = NULL;

        // Memasukkan kantong ke lemari utama pakai fungsi buatan temanmu
        insertNodeToList(l, newNode, err);
        
        if (*err == ERR_OK) {
            Serial.print(F("BERHASIL! Barang '"));
            Serial.print(itemName);
            Serial.println(F("' resmi ditambahkan ke gudang."));
        } else {
            Serial.println(F("GAGAL: Gudang sudah penuh (Maksimal Kapasitas)."));
            free(newNode); // Buang memori kalau gagal masuk
            break;
        }
    }
}

void tarikBarangAdmin(InventoryList *l, ErrorCode *err) {
    *err = ERR_OK;

    if (l == NULL || l->head == NULL) {
        Serial.println(F("\nProses Gagal: Gudang masih kosong."));
        return;
    }

    Serial.println(F("\n=== SESI TARIK BARANG PERMANEN ==="));
    Serial.println(F("[INFO] Ketik ID '0' untuk selesai dan kembali ke menu."));

    // Looping agar Admin bisa menghapus beberapa barang sekaligus
    while (true) {
        char buffer[MAX_LENGTH + 1];
        uint8_t targetId = 0;

        Serial.print(F("\nMasukkan ID Barang yang ingin ditarik: "));
        readSerialString(buffer, err);
        if (*err != ERR_OK) return;
        
        stringToInt(buffer, &targetId, err);
        if (*err != ERR_OK) return;

        // Fitur Keluar Cepat
        if (targetId == 0) {
            Serial.println(F("Sesi penarikan barang diakhiri."));
            break; 
        }

        InventoryNode *curr = l->head;
        InventoryNode *prev = NULL;
        bool barangDitemukan = false;

        // Looping mencari barang di dalam database
        while (curr != NULL) {
            if (curr->data.itemId == targetId) {
                barangDitemukan = true;

                // CEK KEAMANAN: Jangan izinkan tarik kalau masih dipinjam!
                if (curr->data.stock.borrowed > 0) {
                    Serial.print(F("GAGAL: Barang '"));
                    Serial.print(curr->data.itemName);
                    Serial.println(F("' masih ada yang dipinjam. Tidak bisa ditarik!"));
                } else {
                    // Kasih info barang apa yang barusan dihapus biar Admin yakin
                    Serial.print(F("BERHASIL! Barang '"));
                    Serial.print(curr->data.itemName);
                    Serial.println(F("' ditarik dan dihapus permanen."));
                    
                    // Hapus pakai fungsi temanmu
                    deleteInventoryNode(l, curr, prev);
                }
                break; // Keluar dari loop pencarian barang, lanjut minta ID berikutnya
            }
            prev = curr;
            curr = curr->next;
        }

        // Kalau Admin masukin ID ngasal
        if (!barangDitemukan) {
            Serial.println(F("GAGAL: ID Barang tersebut tidak ditemukan di gudang."));
        }
        
        // Pengecekan Ekstra: Kalau ternyata gudang jadi kosong melompong setelah penghapusan
        if (l->head == NULL) {
            Serial.println(F("\n[INFO] Gudang sekarang sudah kosong sepenuhnya. Sesi otomatis diakhiri."));
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
        Serial.println(F("\n==============================="));
        Serial.println(F("  MENU ADMIN (PEMILIK BARANG)  "));
        Serial.println(F("==============================="));
        Serial.println(F("1. Lihat Daftar Barang di Gudang"));
        Serial.println(F("2. Tambah / Titip Barang Baru"));
        Serial.println(F("3. Tarik Barang Permanen"));
        Serial.println(F("4. Logout"));
        Serial.print(F("Pilih menu (1-4): "));

        readSerialString(buffer, &err);
        if (err != ERR_OK) continue;

        char choice = buffer[0];
        clearSerial();

        switch (choice) {
            case '1': 
                tampilkanBarangTersedia(l, &err); // Manggil file fiturmu sebelumnya
                break;
            case '2': 
                tambahBarangAdmin(l, &err);
                break;
            case '3': 
                tarikBarangAdmin(l, &err);
                break;
            case '4': 
                Serial.println(F("\nLogout berhasil. Kembali ke menu login..."));
                diMenuAdmin = false; 
                break;
            default: 
                Serial.println(F("\nPilihan tidak valid. Silakan coba lagi.")); 
                break;
        }
    }
}