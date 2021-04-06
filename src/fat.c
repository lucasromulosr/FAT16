#include "fat.h"

void init(){
    
    FILE* file = fopen("fat.part", "w");
    
    cluster_t* boot = init_boot();
    table_t* table = init_fat();
    cluster_t* cluster = init_cluster();

    // init fat table
    for(int i = 0; i < BOOT; i++)
        table->fat[i] = 0xfffd;
    for(int i = BOOT; i < BOOT + TABLE; i++)
        table->fat[i] = 0xfffe;
    for(int i = BOOT + TABLE; i < FAT; i++)
        table->fat[i] = 0x0000;
    
    // write boot
    for(int i = 0; i < BOOT; i++)
        fwrite(boot, 1, CLUSTER, file);
    
    // write fat table
    fwrite(table, 1, TABLE*CLUSTER, file);

    // wrtite empty clusters
    for(int i = 0; i < OTHERS; i++)
        fwrite(cluster, 1, CLUSTER, file);

    fclose(file);
}

void load(table_t* table, cluster_t* current){

    FILE* file = fopen("fat.part", "rw");
    
    // load fat table and root dir
    table = get_fat(file);
    current = get_root(file);
    
    fclose(file);
}
