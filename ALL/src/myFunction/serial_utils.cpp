#include "serial_utils.hpp"

void clearSerial() {
    unsigned long last = millis();

    while ((unsigned long)(millis() - last) < MAX_SERIAL_IDLE_TIMEOUT) {

        while (Serial.available()) {
            Serial.read();
            last = millis();
        }
    }
}

void readSerialString(char *buffer, ErrorCode *err)
{
    if (buffer == NULL) {
        *err = ERR_DATA_NULL;
        return;
    }

    Serial.setTimeout(MAX_SERIAL_INPUT_TIMEOUT);

    size_t n = Serial.readBytesUntil('\n', buffer, MAX_LENGTH - 1);

    if (n == 0) {
        buffer[0] = '\0';
        *err = ERR_SERIAL_INPUT_TIMEOUT;
        return;
    }

    buffer[n] = '\0';

    // hapus CR/LF
    buffer[strcspn(buffer, "\r\n")] = '\0';

    clearSerial();

    *err = ERR_OK;
}

void stringToInt(const char *buffer, uint8_t *res, ErrorCode *err) {
    if (buffer == NULL || res == NULL) {
        *err = ERR_DATA_NULL;
        return;
    }

    *res = 0;   // default

    // empty string
    if (*buffer == '\0') {
        *err = ERR_STRING_EMPTY;
        return;
    }

    // 2 byte jika terjadi overflow
    uint16_t value = 0;
    while (*buffer != '\0') {

        // non numeric
        if (*buffer < '0' || *buffer > '9') {
            *err = ERR_NON_UNSIGNED_NUMERIC;
            return;
        }

        value = (value * 10) + (*buffer - '0');

        // overflow uint8
        if (value > MAX_INTEGER) {
            *err = ERR_OVERFLOW_UINT8;
            return;
        }

        buffer++;
    }

    *res = (uint8_t)value;
    *err = ERR_OK;
}