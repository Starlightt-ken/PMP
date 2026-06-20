#include <Arduino.h>
#include "myFunction/serial_bridge.h" 
#include "myFunction/inventory_system.h" 
#include "myFunction/menu_utama.h"

// Global Variable untuk gudang
InventoryList warehouse;

void setup() {
    // Menyalakan komunikasi ke laptop 
    serial_mulai(); 
    
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    
    delay(1000); 

    // Eksekusi data awal
    inisialisasiSistem(&warehouse);
}

void loop() {
    jalankanMenuUtama(&warehouse);
}