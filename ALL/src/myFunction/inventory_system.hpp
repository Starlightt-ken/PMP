#ifndef INVENTORY_SYSTEM_HPP
#define INVENTORY_SYSTEM_HPP

#include "inventory_operation.hpp"
#include "memory_management.hpp"
#include "errors.hpp"

// ======================= HELPER VARIABLE =======================

// error flag
extern ErrorCode ERROR_FLAG;

// Flag untuk menampilkan menu
extern bool shouldDisplayMenu;

// ======================= CORE SYSTEM =======================

// Memproses menu utama
void process(InventoryList *l);

// buat cek cek aja
// Tambahkan baris ini kalau belum ada!
void inisialisasiSistem(InventoryList *l);

// membuat list baru
void createList(InventoryList *l);

#endif