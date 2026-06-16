#ifndef PINJAM_BARANG_HPP
#define PINJAM_BARANG_HPP

#include <Arduino.h>
#include "types.hpp"
#include "helper.hpp"
#include "serial_utils.hpp"

// Fungsi murni hanya untuk menangani peminjaman barang
void pinjamBarang(InventoryList *l, ErrorCode *err);

#endif