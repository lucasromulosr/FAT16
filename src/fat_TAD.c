#include "fat.h"

/**** flag fat_loaded ****/
int fat_loaded = FALSE;

int get_fat_loaded(){ return fat_loaded; }
void set_fat_loaded(int i){ fat_loaded = i; }

/**** convertions ****/
uint8_t* ctohex(char* name, uint8_t* filename){
    
    for(int i = 0; i < 18; i++) filename[i] = 0x0000;
    
    for(int i = 0; i < strlen(name); i++)
        filename[i] = (uint8_t)name[i];

    return filename;
}

char* hextoc(uint8_t* filename, char* name){
    
    for(int i = 0; i < 18; i++)
        name[i] = '\0';
    
    for(int i = 0; i < sizeof(filename); i++)
        name[i] = (char)filename[i];
    
    return name;
}


/**** init ****/
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

dir_t* init_dir_entry(char* name, uint16_t first_block){ // TODO

    dir_t* dir;
    dir = (dir_t*) malloc (sizeof(dir_t));
    
    for(int i = 0; i < 7; i++)
        dir->reserved[i] = 0x0000;
    
    ctohex(name, dir->filename);
    dir->attributes = 0x0000;
    dir->first_block = first_block;
    dir->size = 0x0000;     // ver se vai mudar pra 2
    
    return dir;
}

cluster_t init_dir(uint8_t root, uint8_t first_block){ // TODO
    
    cluster_t cluster;
    
    cluster.dir[0] = *init_dir_entry(".", first_block);
    cluster.dir[1] = *init_dir_entry("..", root);
    
    for(int i = 2; i < CLUSTER/sizeof(dir_t); i++)
        cluster.dir[i] = *init_dir_entry("", 0x0000);
    
    return cluster;
}


/**** get ****/
uint16_t get_first_fat_empty(table_t* table){
    
    for(int i = BOOT+TABLE; i < FAT; i++)
        if(table->fat[i] == 0x0000)
            return i;
    
    printf("Theres no more space in the system!!\n");
    return FALSE;
}

uint8_t get_first_dir_empty(cluster_t* current){
    
    for(int i = 0; i < CLUSTER/sizeof(dir_t); i++)
        if(current->dir[i].filename[0] == 0)
            return i;
    
    printf("Theres no more space in the directory!!\n");
    return FALSE;
}

cluster_t* get_root(FILE* file){
    
    cluster_t* cluster = init_cluster();
    
//     // jumps boot and fat table
//     fseek(file, BOOT*CLUSTER, SEEK_SET);
//     fseek(file, TABLE*CLUSTER, SEEK_CUR);
    
    fread(cluster, 1, sizeof(cluster_t), file);
    
    return cluster;
}

table_t* get_fat(FILE* file){
    
    table_t* table = init_fat();
    
    // jumps boot
    fseek(file, BOOT*CLUSTER, SEEK_SET);
    
    fread(table, 1, TABLE*CLUSTER, file);
    
    return table;
}
