#ifndef USER_HPP
#define USER_HPP

#include <Arduino.h>

// ============================================================
//  DIET MEMORI: Kapasitas Disesuaikan
// ============================================================
#define MAX_USERS 5   
#define MAX_STR   12  

// ============================================================
//  DEFINISI ROLE
// ============================================================
typedef enum {
    ROLE_PIC      = 1,   
    ROLE_ADMIN    = 2,   
    ROLE_PEMINJAM = 3    
} Role;

// ============================================================
//  STRUCT USER
// ============================================================
typedef struct {
    char username[MAX_STR];
    char password[MAX_STR];
    Role role;
} User;

// ============================================================
//  ARRAY USER GLOBAL
// ============================================================
extern User  daftarUser[MAX_USERS];
extern int   totalUser;
extern int   indexUserAktif;   

// ============================================================
//  HELPER: (Diubah menjadi VOID)
// ============================================================
static inline void printRole(Role r) {
    switch (r) {
        case ROLE_PIC:      Serial.print(F("PIC")); break;
        case ROLE_ADMIN:    Serial.print(F("Admin")); break;
        case ROLE_PEMINJAM: Serial.print(F("Peminjam")); break;
        default:            Serial.print(F("Tidak Dikenal")); break;
    }
}

#endif /* USER_HPP */