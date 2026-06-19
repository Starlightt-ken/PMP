#include "memory_management.h"

// Diubah dari (int &freeRam) menjadi (int *freeRam) agar C murni
void getFreeRam(int *freeRam) {
    extern int __heap_start, *__brkval;
    int v;

    *freeRam = (int)&v - (__brkval == 0
                           ? (int)&__heap_start
                           : (int)__brkval);
}