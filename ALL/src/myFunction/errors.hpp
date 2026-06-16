#ifndef ERRORS_HPP
#define ERRORS_HPP

#include <stdlib.h>

// ======================= ERROR HANDLER =======================
typedef enum {
    ERR_OK = 0,

    // general error
    ERR_DATA_NULL,
    ERR_DATABASE_NULL,

    // linked list helper
    ERR_NODE_ALLOCATION_FAILED,
    ERR_INDEX_INVALID,
    ERR_LIST_FULL,
    ERR_LIST_EMPTY,

    // serial helper
    ERR_SERIAL_INPUT_TIMEOUT,

    // parsing helper
    ERR_STRING_EMPTY,
    ERR_NON_UNSIGNED_NUMERIC,
    ERR_OVERFLOW_UINT8,

    // inventory validation
    ERR_ID_EMPTY,
    ERR_STOCK_EMPTY,
    ERR_NAME_EMPTY,
    ERR_OWNER_EMPTY,
    ERR_PIC_EMPTY,

    ERR_COUNT
} ErrorCode;

#endif