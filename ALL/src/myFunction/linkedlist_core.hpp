#ifndef LINKEDLIST_CORE_HPP
#define LINKEDLIST_CORE_HPP

#include "types.hpp"
#include "helper.hpp"

// fungsi utama hapus list
void destroyListCore(InventoryList *l, ErrorCode *err);

// fungsi utama insert node ke list
void insertNodeToList(InventoryList *l, InventoryNode *newNode, ErrorCode *err);

// helper untuk delete node
void deleteInventoryNode(InventoryList *l, InventoryNode *curr, InventoryNode *prev);

#endif