#include <Arduino.h>
#include "myFunction/inventory_system.hpp" 
#include "myFunction/menu_utama.hpp"     // Tempat fungsi jalankanMenuUtama()

// Bikin satu gudang utama (Global Variable)
InventoryList warehouse;

void setup() {
    // Mulai komunikasi serial
    Serial.begin(115200);
    
    // Inisialisasi bawaan (opsional, menyesuaikan kodingan awalmu)
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    // Kasih waktu 1 detik buat Arduino siap-siap
    delay(1000);

    // Siapkan memori gudang pakai fungsi temanmu
    createList(&warehouse);

    Serial.println(F("\n=================================="));
    Serial.println(F(" SISTEM INVENTARIS LAB MENYALA! "));
    Serial.println(F("=================================="));
}

void loop() {
    // Jalankan resepsionis menu utama, serahkan gudang kepadanya
    jalankanMenuUtama(&warehouse);
}