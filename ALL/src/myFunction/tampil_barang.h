#ifndef TAMPIL_BARANG_H
#define TAMPIL_BARANG_H
#include "types.h"
#include "errors.h"
#include <stdbool.h>
#include "ui.h"

//fungsi untuk menampilkan barang yang tersedia

void tampilkanBarangTersedia(const InventoryList *l, ErrorCode *err);

#endif