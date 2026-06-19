#ifndef INVENTORY_OPERATION_H
#define INVENTORY_OPERATION_H

#include <stdint.h>
#include "types.h"
#include "errors.h"
#include <stdbool.h>

typedef enum {
    CMD_SEARCH,
    CMD_DELETE,
    CMD_UPDATE_ITEM,
    CMD_UPDATE_STOCK
} MenuCommand;

void destroyList(InventoryList *l, ErrorCode *err);
void insertList(InventoryList *l, ErrorCode *err);
void runInventoryMenu(InventoryList *l, MenuCommand command, ErrorCode *err);
void processById(InventoryList *l, uint8_t targetId, MenuCommand command, ErrorCode *err);
void processByName(InventoryList *l, const char *targetName, MenuCommand command, ErrorCode *err);
void modifyItemInventoryNode(InventoryNode *curr, ErrorCode *err);
void modifyStockInventoryNode(InventoryNode *curr, ErrorCode *err);

#endif