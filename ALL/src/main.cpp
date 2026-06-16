#include "myFunction/inventory_system.hpp"
#include "myFunction/user_general.hpp" 

InventoryList warehouse; 

// ======================= FREE RAM CHECK =======================

void setup() {
    Serial.begin(115200); 
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    delay(1000);

    createList(&warehouse);
    
}

void loop() {
    process(&warehouse); 
    menuUserGeneral(&warehouse);
}