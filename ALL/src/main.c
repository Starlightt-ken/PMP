#include <Arduino.h>
#include "myFunction/serial_bridge.h" // Wajib panggil jembatan
#include "myFunction/inventory_system.h" 
#include "myFunction/menu_utama.h"

// Global Variable untuk gudang
InventoryList warehouse;

void setup() {
    // Menyalakan komunikasi ke laptop lewat jembatan
    serial_mulai(); 
    
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