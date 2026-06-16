#ifndef INVENTORY_OPERATION_HPP
#define INVENTORY_OPERATION_HPP

#include <Arduino.h>
#include "helper.hpp"
#include "serial_utils.hpp"
#include "ui.hpp"
#include "linkedlist_core.hpp"

typedef enum {
    CMD_SEARCH,
    CMD_DELETE,
    CMD_UPDATE_ITEM,
    CMD_UPDATE_STOCK
} MenuCommand;

// ======================= LINKEDLIST FUNCTION =======================

// Membuat list kosong
// void createList(InventoryList *l, ErrorCode *err);

// Menghapus seluruh node list
void destroyList(InventoryList *l, ErrorCode *err);

// insert node atau item baru
void insertList(InventoryList *l, ErrorCode *err);

// Menu operasi inventory item
void runInventoryMenu(InventoryList *l, MenuCommand command, ErrorCode *err);

// operasi berdasarkan id
void processById(InventoryList *l, uint8_t targetId, MenuCommand command, ErrorCode *err);

// operasi berdasarkan nama
void processByName(InventoryList *l, const char *targetName, MenuCommand command, ErrorCode *err);

// helper untuk update item
void modifyItemInventoryNode(InventoryNode *curr, ErrorCode *err);

// helper untuk update stock
void modifyStockInventoryNode(InventoryNode *curr, ErrorCode *err);

#endif