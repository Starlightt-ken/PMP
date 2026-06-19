#ifndef USER_H
#define USER_H

#include <stdint.h>
#include "serial_bridge.h"

#define MAX_USERS 5   
#define MAX_STR   12  

typedef enum {
    ROLE_PIC      = 1,   
    ROLE_ADMIN    = 2,   
    ROLE_PEMINJAM = 3    
} Role;

typedef struct {
    char username[MAX_STR];
    char password[MAX_STR];
    Role role;
} User;

extern User  daftarUser[MAX_USERS];
extern int   totalUser;
extern int   indexUserAktif;   

static inline void printRole(Role r) {
    switch (r) {
        case ROLE_PIC:      serial_cetak_teks_flash(PSTR("PIC")); break;
        case ROLE_ADMIN:    serial_cetak_teks_flash(PSTR("Admin")); break;
        case ROLE_PEMINJAM: serial_cetak_teks_flash(PSTR("Peminjam")); break;
        default:            serial_cetak_teks_flash(PSTR("Tidak Dikenal")); break;
    }
}

#endif