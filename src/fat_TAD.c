#include "fat.h"

cluster_t* init_cluster(){
    
    cluster_t* empty_cluster;
    empty_cluster = (cluster_t*) malloc (sizeof(cluster_t));
    
    for(int i = 0; i < CLUSTER; i++)
        empty_cluster->data[i] = 0;
    
    return empty_cluster;
}

cluster_t* init_boot(){
    
    cluster_t* boot;
    boot = init_cluster();
    
    for(int i = 0; i < CLUSTER; i++)
        boot->data[i] = 0x00bb;
    
    return boot;
}

table_t* init_fat(){
    
    table_t* table;
    table = (table_t*) malloc (sizeof(table_t));
    
    for(int i = 0; i < FAT; i++)
        table->fat[i] = 0;
    
    return table;
}

cluster_t* get_root(FILE* file){
    
    cluster_t* cluster = init_cluster();
    
    // jumps boot e fat table
    fseek(file, BOOT*CLUSTER, SEEK_SET);
    fseek(file, TABLE*CLUSTER, SEEK_CUR);
    
    fread(cluster, 1, CLUSTER, file);
    
    return cluster;
}

table_t* get_fat(FILE* file){
    
    table_t* table = init_fat();
    
    // jumps boot
    fseek(file, BOOT*CLUSTER, SEEK_SET);
    
    fread(table, 1, TABLE*CLUSTER, file);
    
    return table;
}
