#include "serial_utils.h"
#include "serial_bridge.h"
#include <string.h>

void clearSerial() {
    // Kita panggil jembatan khusus pembersih buffer
    serial_bersihkan();
}

void readSerialString(char *buffer, ErrorCode *err) {
    if (buffer == NULL) {
        *err = ERR_DATA_NULL;
        return;
    }

    // Memanfaatkan fungsi jembatan yang otomatis menangani timeout & membaca string
    serial_baca_string(buffer, MAX_LENGTH);

    if (buffer[0] == '\0') {
        *err = ERR_SERIAL_INPUT_TIMEOUT;
        return;
    }

    *err = ERR_OK;
}

void stringToInt(const char *buffer, uint8_t *res, ErrorCode *err) {
    if (buffer == NULL || res == NULL) {
        *err = ERR_DATA_NULL;
        return;
    }

    *res = 0;   

    if (*buffer == '\0') {
        *err = ERR_STRING_EMPTY;
        return;
    }

    uint16_t value = 0;
    while (*buffer != '\0') {

        if (*buffer < '0' || *buffer > '9') {
            *err = ERR_NON_UNSIGNED_NUMERIC;
            return;
        }

        value = (value * 10) + (*buffer - '0');

        if (value > MAX_INTEGER) {
            *err = ERR_OVERFLOW_UINT8;
            return;
        }

        buffer++;
    }

    *res = (uint8_t)value;
    *err = ERR_OK;
}