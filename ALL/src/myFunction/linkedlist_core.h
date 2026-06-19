#ifndef LINKEDLIST_CORE_H
#define LINKEDLIST_CORE_H

#include "types.h"
#include "errors.h"

void destroyListCore(InventoryList *l, ErrorCode *err);
void insertNodeToList(InventoryList *l, InventoryNode *newNode, ErrorCode *err);
void deleteInventoryNode(InventoryList *l, InventoryNode *curr, InventoryNode *prev);

#endif