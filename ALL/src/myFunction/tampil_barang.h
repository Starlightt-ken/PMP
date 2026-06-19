#ifndef TAMPIL_BARANG_H
#define TAMPIL_BARANG_H
#include "types.h"
#include "errors.h"
#include <stdbool.h>

//fungsi untuk menampilkan barang yang tersedia

void tampilkanBarangTersedia(const InventoryList *l, ErrorCode *err);
extern void printCategory(ItemCategory cat);
#endif