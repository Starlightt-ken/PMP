#ifndef PENGEMBALIAN_BARANG_HPP
#define PENGEMBALIAN_BARANG_HPP

#include <Arduino.h>
#include "types.hpp"
#include "serial_utils.hpp"

// Fungsi murni hanya untuk menangani pengembalian barang
void kembalikanBarang(InventoryList *l, ErrorCode *err);

#endif