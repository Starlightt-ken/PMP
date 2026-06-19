#ifndef SERIAL_UTILS_H
#define SERIAL_UTILS_H

#include <stdint.h>
#include "errors.h"
#include "config.h"

void clearSerial();
void readSerialString(char *buffer, ErrorCode *err);
void stringToInt(const char *buffer, uint8_t *res, ErrorCode *err);

#endif