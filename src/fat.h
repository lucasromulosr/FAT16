#ifndef __FAT__
#define __FAT__

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define FAT 4096
#define BOOT 2
#define TABLE 8
#define OTHERS (FAT-BOOT-TABLE)

typedef struct boot_t{
    
    int cluster[256];
    
} boot_t;

typedef struct table_t{
    
    uint16_t fat[512];
    
} table_t;

typedef struct other_t{
    
    int cluster[256];
    
} other_t;

typedef struct fat_t{
    
    boot_t boot[BOOT];
    table_t fat[TABLE];
    other_t others[OTHERS];    
    
} fat_t;



void init();






#endif
