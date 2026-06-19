#ifndef INVENTORY_SYSTEM_H
#define INVENTORY_SYSTEM_H

#include <stdbool.h>
#include "types.h"
#include "errors.h"

//helper value
extern ErrorCode ERROR_FLAG;
extern bool shouldDisplayMenu;

//core system   

void process(InventoryList *l);
void inisialisasiSistem(InventoryList *l);
void createList(InventoryList *l);

#endif