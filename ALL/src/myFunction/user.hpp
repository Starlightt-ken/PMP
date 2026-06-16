#ifndef USER_HPP
#define USER_HPP

#include <Arduino.h>

#define MAX_USERS 10
#define MAX_STR   16

// ============================================================
//  DEFINISI ROLE
// ============================================================
typedef enum {
    ROLE_PIC      = 1,   // Person In Charge  (imam)
    ROLE_ADMIN    = 2,   // Admin
    ROLE_PEMINJAM = 3    // Peminjam / Konsumen (Kenny)
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
extern int   indexUserAktif;   // -1 = belum login

// ============================================================
//  HELPER: konversi role ke string
// ============================================================
static inline const char* roleToString(Role r) {
    switch (r) {
        case ROLE_PIC:      return "PIC";
        case ROLE_ADMIN:    return "Admin";
        case ROLE_PEMINJAM: return "Peminjam";
        default:            return "Tidak Dikenal";
    }
}

#endif /* USER_HPP */