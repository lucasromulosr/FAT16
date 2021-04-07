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
    table->fat[BOOT+TABLE] = 0x0001;    // root dir
    
    // write boot
    for(int i = 0; i < BOOT; i++)
        fwrite(boot, 1, CLUSTER, file);
    
    // write fat table
    fwrite(table, 1, TABLE*CLUSTER, file);

    // wrtite empty clusters
    for(int i = 0; i < OTHERS; i++)
        fwrite(cluster, 1, CLUSTER, file);
    
    printf("FAT system created!!\n");

    fclose(file);
}

void load(table_t* table, cluster_t* current){

    FILE* file = fopen("fat.part", "r");
    
    if(file == NULL)
        printf("There is no FAT system in the disk!!\n");
    else{
        // load fat table and root dir
        table = get_fat(file);
        current = get_root(file);
        fclose(file);
        printf("FAT system loaded!!\n");
        set_fat_loaded(TRUE);
    }
}

void mkdir(char* name, table_t* table, cluster_t* current){
    
    if(!get_fat_loaded()){
        printf("Please, load the system first!!\n");
        return;
    }
    
    uint16_t fat_empty = get_first_fat_empty(table);
    if(!fat_empty)
        return;
    printf("Fat empty: %d\n", fat_empty);
    
    uint8_t dir_empty = get_first_dir_empty(current);
//     if(!dir_empty)
//         return;
    printf("Dir empty: %d\n", dir_empty);


    
}





void ls(cluster_t* current){
    
    for(int i = 0; i < CLUSTER/sizeof(dir_t); i++)
//         printf("%d \n", i);
        printf("%d, %hhn \n", i, current->dir[0].filename);
    
}
