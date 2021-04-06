#ifndef __FAT__
#define __FAT__

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define FAT 4096
#define CLUSTER 1024
#define BOOT 2
#define TABLE 8
#define OTHERS (FAT-BOOT-TABLE)

typedef struct table_t{
    
    uint16_t fat[FAT];
    
} table_t;

typedef struct dir_t{
    uint8_t filename[18];
    uint8_t attributes;
    uint8_t reserved[7];
    uint16_t first_block;
    uint32_t size;
} dir_t;

typedef union cluster_t{
    
    dir_t dir[CLUSTER/sizeof(dir_t)];
    uint8_t data[CLUSTER];

} cluster_t ;



/**** init ****/
cluster_t* init_cluster();
cluster_t* init_boot();
table_t* init_fat();

/**** get ****/
table_t* get_fat(FILE*);
cluster_t* get_root(FILE*);




void init();
void load(table_t*, cluster_t*);






#endif
