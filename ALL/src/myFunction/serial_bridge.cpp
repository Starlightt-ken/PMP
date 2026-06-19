#include <Arduino.h>
#include "serial_bridge.h"

extern "C" {
    void serial_mulai(void) {
        Serial.begin(115200);
    }

    void serial_cetak_teks(const char* teks) {
        Serial.print(teks);
    }

    void serial_cetak_teks_ln(const char* teks) {
        Serial.println(teks);
    }

    // Mengubah teks C murni PSTR menjadi format Flash C++
    void serial_cetak_teks_flash(const char* teks_flash) {
        Serial.print((const __FlashStringHelper*)teks_flash);
    }

    void serial_cetak_teks_ln_flash(const char* teks_flash) {
        Serial.println((const __FlashStringHelper*)teks_flash);
    }

    void serial_cetak_angka(int angka) {
        Serial.print(angka);
    }

    void serial_cetak_angka_ln(int angka) {
        Serial.println(angka);
    }

    void serial_bersihkan(void) {
        while (Serial.available() > 0) {
            Serial.read();
        }
    }

    void serial_baca_string(char* buffer, uint8_t max_len) {
        uint8_t index = 0;
        
        // Looping akan menahan program SELAMANYA sampai user menekan Enter
        while (true) {
            if (Serial.available()) {
                char c = Serial.read();
                
                // Abaikan sisa Enter/Spasi jika itu adalah karakter pertama
                if (index == 0 && (c == '\n' || c == '\r' || c == ' ')) {
                    continue;
                }
                
                // Jika ditekan Enter, pembacaan selesai
                if (c == '\n' || c == '\r') {
                    break;
                }
                
                // Menyimpan karakter ke dalam buffer
                if (index < max_len - 1) {
                    buffer[index++] = c;
                }
            } else {
                // Kasih jeda sedikit agar IC Arduino tidak kerja terlalu berat saat menunggu
                delay(10);
            }
        }
        buffer[index] = '\0';
        
        delay(10); 
        serial_bersihkan(); 
    }
}