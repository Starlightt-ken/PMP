#ifndef UI_H
#define UI_H

#include "types.h"
#include "errors.h"

void displayList(const InventoryList *l, ErrorCode *err);
void displayItemData(const ItemData *item);
void printErrorMessage(ErrorCode err);
void printCategory(ItemCategory cat);
void printLocation(ItemLocation loc);
void printMemory(const InventoryList *l);

#endif