#include "linkedlist_core.hpp"

void destroyListCore(InventoryList *l, ErrorCode *err) {
    if (l == NULL) {
        *err = ERR_DATABASE_NULL;
        return;
    }

    if (l->head == NULL || l->size == 0) {
        *err = ERR_LIST_EMPTY;
        return;
    }

    InventoryNode *curr = l->head;

    while (curr != NULL) {
        InventoryNode *temp = curr;
        curr = curr->next;
        free(temp);
    }

    l->head = NULL;
    l->size = 0;

    *err = ERR_OK;
}

void insertNodeToList(InventoryList *l, InventoryNode *newNode, ErrorCode *err) {
    if (l == NULL) {
        *err = ERR_DATABASE_NULL;
        return;
    }

    if (l->size >= MAX_CAPACITY) {
        *err = ERR_LIST_FULL;
        return;
    }

    if (l->head == NULL) {
        l->head = newNode;
    } else {
        InventoryNode *curr = l->head;

        while (curr->next != NULL) {
            curr = curr->next;
        }

        curr->next = newNode;
    }

    l->size++;
    *err = ERR_OK;
}

void deleteInventoryNode(InventoryList *l, InventoryNode *curr, InventoryNode *prev) {
    //node yang dihapus adalah head
    if (prev == NULL) {
        l->head = curr->next;
    } 

    //node yang dihapus ada di tengah atau di akhir
    else {
        prev->next = curr->next;
    }

    free(curr);
    l->size--;
}