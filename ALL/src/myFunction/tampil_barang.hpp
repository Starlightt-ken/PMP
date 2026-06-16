#ifndef TAMPIL_BARANG_HPP
#define TAMPIL_BARANG_HPP

#include <Arduino.h>
#include "types.hpp"
#include "helper.hpp"

// Fungsi murni hanya untuk menampilkan barang yang tersedia
void tampilkanBarangTersedia(const InventoryList *l, ErrorCode *err);

// Deklarasi eksternal agar bisa memanggil fungsi printCategory milik temanmu
extern void printCategory(ItemCategory cat);

#endif