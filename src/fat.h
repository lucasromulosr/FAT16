#ifndef __FAT__
#define __FAT__

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define TRUE 1
#define FALSE 0

#define FAT 4096
#define CLUSTER 1024
#define BOOT 2
#define TABLE 8
#define OTHERS (FAT-BOOT-TABLE)

typedef struct table_t{
    
    uint16_t fat[FAT];
    
} table_t;

typedef struct dir_t{
    uint8_t filename[18];   // 18
    uint8_t attributes;
    uint8_t reserved[7];    // 7
    uint16_t first_block;
    uint32_t size;
} dir_t;

typedef union cluster_t{

    dir_t dir[CLUSTER/sizeof(dir_t)];
    uint8_t data[CLUSTER];

} cluster_t ;

/**** flag fat_loaded ****/
int get_fat_loaded();
void set_fat_loaded(int);

/**** convertions ****/
uint8_t* ctohex(char*, uint8_t*);
char* hextoc(uint8_t*, char*);

/**** init ****/
cluster_t* init_cluster();
cluster_t* init_boot();
table_t* init_fat();
dir_t* init_dir_entry(char*, uint8_t, uint16_t);  
cluster_t init_dir(uint16_t, uint16_t);

/**** get ****/
table_t* get_fat();
cluster_t* get_cluster(int);
uint16_t get_first_fat_empty(table_t*);
uint8_t get_first_dir_empty(cluster_t*);

/**** set ****/
void set_fat(table_t*);
void set_cluster(int, cluster_t*);



void help();
void init();
void load();
void ls();
void mkdir(char*);
void create(char*);
void unlink(char*);






#endif
