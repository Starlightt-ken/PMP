#ifndef UI_HPP
#define UI_HPP

#include <Arduino.h>
#include "helper.hpp"
#include "serial_utils.hpp"
#include "memory_management.hpp"

// ======================= USER INTERFACE =======================

// menampilkan menu utama
void displayMenu();

// menampilkan seluruh item
void displayList(const InventoryList *l, ErrorCode *err);

// helper untuk print data dari item
void displayItemData(const ItemData *item);

// Menampilkan pesan error
void printErrorMessage(ErrorCode err);

// Menampilkan kategori
void printCategory(ItemCategory cat);

// Menampilkan lokasi
void printLocation(ItemLocation loc);

// Menampilkan semua memori sistem
void printMemory(const InventoryList *l);

// Meminta input ke pengguna
void getInput(InventoryList *l, InventoryNode *newNode, ErrorCode *err);

#endif