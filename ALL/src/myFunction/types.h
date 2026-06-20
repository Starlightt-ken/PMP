#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include "config.h"

typedef enum {
    KATEGORI_SENSOR = 0,
    KATEGORI_AKTUATOR,
    KATEGORI_MIKROKONTROLLER,

    KATEGORI_COUNT
} ItemCategory;

typedef enum {
    LOKASI_LEMARI = 0,
    LOKASI_MEJA,

    LOKASI_COUNT
} ItemLocation;

typedef struct {
    uint8_t totalStock;
    uint8_t borrowed;
    uint8_t broken;
} StockInfo;

typedef struct {
    uint8_t itemId;
    char itemName[MAX_LENGTH + 1]; // 19 bytes

    uint8_t category : 4;          // 0.5 byte
    uint8_t location : 4;          // 0.5 byte

    StockInfo stock;               // 3 bytes

    int8_t ownerIndex; 
    int8_t picIndex;   
} ItemData;

typedef struct InventoryNode {
    ItemData data;
    struct InventoryNode *next;
} InventoryNode;

typedef struct {
    InventoryNode *head;
    uint8_t size;
} InventoryList;

#endif