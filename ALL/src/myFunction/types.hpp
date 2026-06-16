#ifndef TYPES_HPP
#define TYPES_HPP

#include <stdint.h>
#include "config.hpp"

// category type
typedef enum {
    KATEGORI_SENSOR = 0,
    KATEGORI_AKTUATOR,
    KATEGORI_MIKROKONTROLLER,

    KATEGORI_COUNT
} ItemCategory;

// location type
typedef enum {
    LOKASI_LEMARI = 0,
    LOKASI_MEJA,

    LOKASI_COUNT
} ItemLocation;

// stock information
typedef struct {
    uint8_t totalStock;
    uint8_t borrowed;
    uint8_t broken;
} StockInfo;

// item data structure
typedef struct {
    uint8_t itemId;

    char itemName[MAX_LENGTH + 1];

    uint8_t category : 4;
    uint8_t location : 4;

    StockInfo stock;

    char owner[MAX_LENGTH + 1];
    char pic[MAX_LENGTH + 1];

} ItemData;

// linked list node
typedef struct InventoryNode {
    ItemData data;
    struct InventoryNode *next;
} InventoryNode;

// linked list container
typedef struct {
    InventoryNode *head;
    uint8_t size;
} InventoryList;

#endif