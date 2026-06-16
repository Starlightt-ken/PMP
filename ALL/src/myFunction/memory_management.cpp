#include "memory_management.hpp"

void getFreeRam(int &freeRam) {
    extern int __heap_start, *__brkval;
    int v;

    freeRam = (int)&v - (__brkval == 0
                           ? (int)&__heap_start
                           : (int)__brkval);
}