#include "inventory_system.hpp"

ErrorCode ERROR_FLAG = ERR_OK;

bool shouldDisplayMenu = true;

void createList(InventoryList *l) {
    if (l == NULL) {
        ERROR_FLAG = ERR_DATABASE_NULL;
        return;
    }

    l->head = NULL;
    l->size = 0;

    ERROR_FLAG = ERR_OK;
}

void process(InventoryList *l) {
    if (l == NULL) {
        ERROR_FLAG = ERR_DATABASE_NULL;
        return;
    }

    // show menu
    if (shouldDisplayMenu) {
        displayMenu();
        shouldDisplayMenu = false;
    }

    // tunggu input
    if (!Serial.available()) {
        return;
    }

    char choice = Serial.read();
    ERROR_FLAG = ERR_OK;

    switch (choice) {
        // menambah data
        case '1':{
            insertList(l, &ERROR_FLAG);
            if (ERROR_FLAG != ERR_OK) {
                printErrorMessage(ERROR_FLAG);
            }

            shouldDisplayMenu = true;
            clearSerial();
            break;
        }

        // menghapus data
        case '2': {
            runInventoryMenu(l, CMD_DELETE, &ERROR_FLAG);
            if (ERROR_FLAG != ERR_OK) {
                printErrorMessage(ERROR_FLAG);
            }

            shouldDisplayMenu = true;
            clearSerial();
            break;
        }

        // mencari data
        case '3': {
            runInventoryMenu(l, CMD_SEARCH, &ERROR_FLAG);
            if (ERROR_FLAG != ERR_OK) {
                printErrorMessage(ERROR_FLAG);
            }

            shouldDisplayMenu = true;
            clearSerial();
            break;
        }

        // memperbarui stock
        case '4': {
            runInventoryMenu(l, CMD_UPDATE_STOCK, &ERROR_FLAG);
            if (ERROR_FLAG != ERR_OK) {
                printErrorMessage(ERROR_FLAG);
            }

            shouldDisplayMenu = true;
            clearSerial();
            break;
        }

        // memperbarui data
        case '5': {
            runInventoryMenu(l, CMD_UPDATE_ITEM, &ERROR_FLAG);
            if (ERROR_FLAG != ERR_OK) {
                printErrorMessage(ERROR_FLAG);
            }

            shouldDisplayMenu = true;
            clearSerial();
            break;
        }

        // menampilkan semua data
        case '6': {
            displayList(l, &ERROR_FLAG);
            if (ERROR_FLAG != ERR_OK) {
                printErrorMessage(ERROR_FLAG);
            }

            shouldDisplayMenu = true;
            clearSerial();
            break;
        }

        case '7': {
            destroyList(l, &ERROR_FLAG);
            if (ERROR_FLAG != ERR_OK) {
                printErrorMessage(ERROR_FLAG);
            }
            shouldDisplayMenu = true;
            clearSerial();
            break;
        }

        case '8': {
            printMemory(l);
            shouldDisplayMenu = true;
            clearSerial();
            break;
        }

        case '9': {
            loadInitialData(l);
            if (ERROR_FLAG != ERR_OK) {
                printErrorMessage(ERROR_FLAG);
            }
            shouldDisplayMenu = true;
            clearSerial();
            break;
        }

        default: {
            Serial.println(F("\nPilihan salah"));
            shouldDisplayMenu = true;
            break;
        }
    }
}

void loadInitialData(InventoryList *l) {
    if (l == NULL) {
        ERROR_FLAG = ERR_DATABASE_NULL;
        return;
    }

    Serial.println(F("\n===== LOAD INITIAL DATA ====="));

    for (uint8_t i = 0; i < MAX_CAPACITY; i++) {
        // Langsung alokasikan Node di Heap, tanpa temp variable di Stack!
        InventoryNode *newNode = (InventoryNode*) malloc(sizeof(InventoryNode));
        if (newNode == NULL) {
            ERROR_FLAG = ERR_NODE_ALLOCATION_FAILED;
            return;
        }

        // Inisialisasi nol langsung di heap
        memset(newNode, 0, sizeof(InventoryNode));

        // Isi data langsung ke target heap
        newNode->data.itemId = i + 1;
        newNode->data.category = i % KATEGORI_COUNT;
        newNode->data.location = i % LOKASI_COUNT;
        newNode->data.stock.totalStock = 10;

        // Menggunakan fungsi string yang lebih ringan dibanding snprintf jika memungkinkan
        strcpy_P(newNode->data.itemName, PSTR("Item_"));
        // Atau jika tetap butuh angka:
        itoa(i + 1, newNode->data.itemName + strlen(newNode->data.itemName), 10);

        strcpy(newNode->data.owner, newNode->data.itemName); // contoh copypaste hemat
        strcpy(newNode->data.pic, newNode->data.itemName);

        // Bypass insertAt yang lambat, langsung sambungkan di akhir list
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
    }
    ERROR_FLAG = ERR_OK;
}