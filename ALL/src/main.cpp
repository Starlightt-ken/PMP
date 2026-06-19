#include <Arduino.h>
#include "myFunction/inventory_system.hpp" 
#include "myFunction/menu_utama.hpp"

// Global Variable untuk gudang
InventoryList warehouse;

void setup() {
    Serial.begin(115200);
    
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    
    delay(1000); // Tunggu Arduino stabil

    // Eksekusi data awal
    inisialisasiSistem(&warehouse);
}

void loop() {
    // Serahkan kontrol ke lobi utama
    jalankanMenuUtama(&warehouse);
}