#include "fat.h"

table_t* table;
cluster_t* current;

char file_path[50] = "fat.part";

void init(){
    
    FILE* file = fopen(file_path, "w");
    
    cluster_t* boot = init_boot();
    table = init_fat();
    cluster_t root = init_dir(0xffff, 0xffff);
    cluster_t* cluster = init_cluster();
    
    // init fat table
    for(int i = 0; i < BOOT; i++)
        table->fat[i] = 0xfffd;
    for(int i = BOOT; i < BOOT + TABLE; i++)
        table->fat[i] = 0xfffe;
    for(int i = BOOT + TABLE; i < FAT; i++)
        table->fat[i] = 0x0000;
    table->fat[BOOT+TABLE] = 0xffff;    // root dir
    
    // write boot
    for(int i = 0; i < BOOT; i++)
        fwrite(boot, 1, CLUSTER, file);
    
    // write fat table
    fwrite(table, 1, TABLE*CLUSTER, file);
    
    //write root
    fwrite(&root, 1, sizeof(root), file);
    
    // wrtite empty clusters
    for(int i = 0; i < OTHERS-1; i++)
        fwrite(cluster, 1, CLUSTER, file);
    
    printf("FAT system created!!\n");

    fclose(file);
}

void load(){

    FILE* file = fopen(file_path, "r");
    
    if(file == NULL)
        printf("There is no FAT system in the disk!!\n");
    
    else{
        fclose(file);
        // load fat table and root dir
        table = get_fat();
        current = get_cluster(0);
        printf("FAT system loaded!!\n");
        // set flag fat_loaded, close file
        set_fat_loaded(TRUE);
    }
}

void mkdir(char* name){
    
    // check flag fat loaded
    if(!get_fat_loaded()){
        printf("Please, load the system first!!\n");
        return;
    }
    
    // finds 1st empty cluster
    uint16_t fat_empty = get_first_fat_empty(table);
    if(!fat_empty)
        return;
    printf("Fat empty: %d\n", fat_empty);
    
    // finds 1st empty dir entry
    uint8_t dir_empty = get_first_dir_empty(current);
    if(!dir_empty)
        return;
    printf("Dir empty: %d\n", dir_empty);
    
    // creates and add new dir to current dir
    dir_t dir;
    dir = *init_dir_entry(name, 0x00, (uint16_t)fat_empty);
    current->dir[dir_empty] = dir;
    
    // creates dir cluster
    cluster_t cluster =
        init_dir(current->dir[0].first_block, fat_empty);
    
    // updates fat
    int fat_pos = (int)fat_empty + BOOT+TABLE;
    table->fat[fat_pos] = (uint16_t)fat_empty;
    set_fat(table);    
    
    // saves new dir cluster
    set_cluster((int)fat_empty, &cluster);
    
    // updates current dir cluster
    current->dir[0].size += 0x0001;
    printf(". dir size: %d <---\n", current->dir[0].size);
    uint16_t current_first_block = current->dir[0].first_block;
    int current_pos = 
        current_first_block == 0xffff ? 0 : current_first_block;
    set_cluster((int)current_pos, current);
}

void create(char* name){
    
    // check flag fat loaded
    if(!get_fat_loaded()){
        printf("Please, load the system first!!\n");
        return;
    }
    
    // finds 1st empty cluster
    uint16_t fat_empty = get_first_fat_empty(table);
    if(!fat_empty)
        return;
    printf("Fat empty: %d\n", fat_empty);
    
    // finds 1st empty dir entry
    uint8_t dir_empty = get_first_dir_empty(current);
    if(!dir_empty)
        return;
    printf("Dir empty: %d\n", dir_empty);
    
    // creates and add new dir to current dir
    dir_t dir;
    dir = *init_dir_entry(name, 0x01, (uint16_t)fat_empty);
    current->dir[dir_empty] = dir;
    
    // creates file cluster
    cluster_t cluster = *init_cluster();
    
    // updates fat
    int fat_pos = (int)fat_empty + BOOT+TABLE;
    table->fat[fat_pos] = (uint16_t)fat_empty;
    set_fat(table);
    
    // saves new dir cluster
    set_cluster((int)fat_empty, &cluster);
    
    // updates current dir cluster
    current->dir[0].size += 0x0001;
    printf(". dir size: %d <---\n", current->dir[0].size);
    uint16_t current_first_block = current->dir[0].first_block;
    int current_pos = 
        current_first_block == 0xffff ? 0 : current_first_block;
    set_cluster((int)current_pos, current);
}

void unlink(char* name){    // falta tirar o cluster da fat
    
    char filename[18];
    int flag = FALSE;
    
    for(int i = 2; i < (CLUSTER/sizeof(dir_t)) -2; i++){
        hextoc(current->dir[i].filename, filename);
        if(!strcmp(filename, name)){
            flag = TRUE;
            
            int offset = (int)current->dir[i].first_block;
            cluster_t cluster = *init_cluster();
            set_cluster(offset, &cluster);
            
            dir_t dir = *init_dir_entry("", 0x00, 0x0000);
            current->dir[i] = dir;
            
            current->dir[0].size -= 0x0001;
        }
    }
    
    uint16_t current_first_block = current->dir[0].first_block;
    int current_pos = 
        current_first_block == 0xffff ? 0 : current_first_block;
    set_cluster(current_pos, current);
    
    if(flag) printf("Unlinked!!\n");
    else printf("Nothing to unlike!!\n");
}







void ls(){
    
    char name[23];
    
    for(int i = 0; i < CLUSTER/sizeof(dir_t); i++){
        hextoc(current->dir[i].filename, name);
        if(strcmp(name, "")){
            if(current->dir[i].attributes == 0x01)
                strcat(name, ".txt");
            printf("%s \t", name);
        }
    }
    printf("\n");
}
