#include "helper.hpp"

void getAvailableStock(const StockInfo *stock, uint8_t *availableStock) {
    uint16_t used = stock->borrowed + stock->broken;

    if (used > stock->totalStock) {
        *availableStock = 0;
    } else {
        *availableStock = stock->totalStock - used;
    }
}

void isItemIdExist(const InventoryList *l, uint8_t itemId, bool *out, ErrorCode *err) {
    if (l == NULL || out == NULL) {
        *err = ERR_DATA_NULL;
        return;
    }

    // Set default awal
    *out = false;
    *err = ERR_OK;

    for (InventoryNode *curr = l->head; curr != NULL; curr = curr->next) {
        if (curr->data.itemId == itemId) {
            *out = true;
            return;
        }
    }
}