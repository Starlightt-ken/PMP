#ifndef SERIAL_HPP
#define SERIAL_HPP

#include <Arduino.h>
#include "errors.hpp"
#include "config.hpp"

// ======================= SERIAL HELPER =======================

// Membersihkan buffer serial
void clearSerial();

// Membaca string dari serial
void readSerialString(char *buffer, ErrorCode *err);

// Konversi string ke uint8_t
void stringToInt(const char *buffer, uint8_t *res, ErrorCode *err);

#endif