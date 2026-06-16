#ifndef PRIVATE_HELPER_HPP
#define PRIVATE_HELPER_HPP

#include "types.hpp"
#include "errors.hpp"

// ======================= HELPER INTERNAL =======================

// Menghitung total stock
void getAvailableStock(const StockInfo *stock, uint8_t *availableStock);

// Mengecek apakah item ID sudah ada
void isItemIdExist(const InventoryList *l, uint8_t itemId, bool *out, ErrorCode *err);

#endif