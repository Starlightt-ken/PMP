#ifndef SERIAL_BRIDGE_H
#define SERIAL_BRIDGE_H

#include <stdint.h>
#include <avr/pgmspace.h> // Pustaka sihir memori flash untuk C murni

#ifdef __cplusplus
extern "C" {
#endif

void serial_mulai(void);

// Fungsi untuk variabel biasa (Memakai RAM)
void serial_cetak_teks(const char* teks);
void serial_cetak_teks_ln(const char* teks);

// Fungsi khusus untuk Teks Langsung (Memakai Flash Memory)
void serial_cetak_teks_flash(const char* teks_flash);
void serial_cetak_teks_ln_flash(const char* teks_flash);

void serial_cetak_angka(int angka);
void serial_cetak_angka_ln(int angka);
void serial_bersihkan(void);
void serial_baca_string(char* buffer, uint8_t max_len);

#ifdef __cplusplus
}
#endif

#endif