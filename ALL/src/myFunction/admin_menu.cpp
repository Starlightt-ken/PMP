#include "admin_menu.hpp"
#include "linkedlist_core.hpp"
#include "serial_utils.hpp"
#include "helper.hpp"
#include "tampil_barang.hpp" // Pinjam fitur tampil barang buatanmu sebelumnya

// =========================================================
// FUNGSI INTERNAL ADMIN
// =========================================================

void tambahBarangAdmin(InventoryList *l, ErrorCode *err) {
    char buffer[MAX_LENGTH + 1];
    uint8_t itemId, totalStock, categoryId;
    *err = ERR_OK;

    Serial.println(F("\n--- TAMBAH BARANG BARU ---"));
    
    // 1. Minta ID
    Serial.print(F("Masukkan ID Barang (angka): "));
    readSerialString(buffer, err);
    stringToInt(buffer, &itemId, err);

    // 2. Minta Nama
    Serial.print(F("Masukkan Nama Barang: "));
    char itemName[MAX_LENGTH + 1];
    readSerialString(itemName, err);

    // 3. Minta Kategori
    Serial.print(F("Masukkan Kategori (1-5): "));
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
        return;
    }

    // Mengisi data ke dalam kantong
    newNode->data.itemId = itemId;
    strncpy(newNode->data.itemName, itemName, MAX_LENGTH);
    newNode->data.category = categoryId;
    newNode->data.stock.totalStock = totalStock;
    newNode->data.stock.borrowed = 0; // Barang baru pasti belum ada yang pinjam
    newNode->next = NULL;

    // Memasukkan kantong ke lemari utama pakai fungsi buatan temanmu
    insertNodeToList(l, newNode, err);
    
    if (*err == ERR_OK) {
        Serial.print(F("\nBERHASIL! Barang '"));
        Serial.print(itemName);
        Serial.println(F("' resmi ditambahkan ke gudang."));
    } else {
        Serial.println(F("\nGAGAL: Gudang sudah penuh (Maksimal Kapasitas)."));
        free(newNode); // Buang memori kalau gagal masuk
    }
}

void tarikBarangAdmin(InventoryList *l, ErrorCode *err) {
    char buffer[MAX_LENGTH + 1];
    uint8_t targetId;
    *err = ERR_OK;

    if (l == NULL || l->head == NULL) {
        Serial.println(F("\nProses Gagal: Gudang masih kosong."));
        return;
    }

    Serial.println(F("\n--- TARIK BARANG PERMANEN ---"));
    Serial.print(F("Masukkan ID Barang yang ingin ditarik: "));
    
    readSerialString(buffer, err);
    stringToInt(buffer, &targetId, err);

    InventoryNode *curr = l->head;
    InventoryNode *prev = NULL;

    // Looping mencari barang
    while (curr != NULL) {
        if (curr->data.itemId == targetId) {
            
            // CEK KEAMANAN: Jangan izinkan tarik barang kalau masih dipinjam mahasiswa!
            if (curr->data.stock.borrowed > 0) {
                Serial.println(F("\nGAGAL: Barang tidak bisa ditarik karena masih ada unit yang dipinjam!"));
                return;
            }

            // Hapus pakai fungsi temanmu
            deleteInventoryNode(l, curr, prev);
            Serial.println(F("\nBERHASIL! Barang ditarik dan dihapus dari sistem secara permanen."));
            return;
        }
        prev = curr;
        curr = curr->next;
    }
    Serial.println(F("\nGAGAL: ID Barang tidak ditemukan di gudang."));
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