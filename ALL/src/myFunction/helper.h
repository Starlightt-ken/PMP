#ifndef HELPER_H
#define HELPER_H

#include <stdbool.h>
#include <stdint.h>
#include "types.h"
#include "errors.h"

// Menghitung total stock
void getAvailableStock(const StockInfo *stock, uint8_t *availableStock);

// Mengecek apakah item ID sudah ada
void isItemIdExist(const InventoryList *l, uint8_t itemId, bool *out, ErrorCode *err);

#endif