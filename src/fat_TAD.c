#include "fat.h"

/**** flag fat_loaded ****/
int fat_loaded = FALSE;

int get_fat_loaded(){ return fat_loaded; }
void set_fat_loaded(int i){ fat_loaded = i; }
int check_sys_load(){ 
    if(!get_fat_loaded()) printf("Please, load the system first!!\n");
    return get_fat_loaded(); 
}


/**** convertions ****/
void ctohex(int size, char* c, uint8_t* hex){
    for(int i = 0; i < size; i++) hex[i] = (uint8_t)c[i];
}

void hextoc(int size, uint8_t* hex, char* c){
    for(int i = 0; i < size; i++) c[i] = (char)hex[i];
}


/**** init ****/
/** creates a empty cluster **/
cluster_t* init_cluster(){
    
    cluster_t* empty_cluster;
    empty_cluster = (cluster_t*) malloc (sizeof(cluster_t));
    
    for(int i = 0; i < CLUSTER; i++)
        empty_cluster->data[i] = 0;
    
    return empty_cluster;
}

/** creates a bluster w/ value 0xbb **/
// the boot clusters
cluster_t* init_boot(){
    
    cluster_t* boot;
    boot = init_cluster();
    
    for(int i = 0; i < CLUSTER; i++)
        boot->data[i] = 0xbb;
    
    return boot;
}

/** creates a empty FAT table **/
table_t* init_fat(){
    
    table_t* table;
    table = (table_t*) malloc (sizeof(table_t));
    
    for(int i = 0; i < FAT; i++)
        table->fat[i] = 0;
    
    return table;
}

/** creates a new dir entry **/
dir_t* init_dir_entry(char* name, uint8_t attributes, uint16_t first_block){

    dir_t* dir;
    dir = (dir_t*) malloc (sizeof(dir_t));
    
    for(int i = 0; i < 7; i++)
        dir->reserved[i] = 0x0000;
    
    // sets name, att, block and size
    ctohex(18, name, dir->filename);
    dir->attributes = attributes;
    dir->first_block = first_block;
    dir->size = 0x0000;
    
    return dir;
}

/** creates a new dir cluster **/
cluster_t init_dir(uint16_t root, uint16_t first_block){
    
    cluster_t cluster;
    
    // creates . and ..
    cluster.dir[0] = *init_dir_entry(".", 0x00, first_block);
    cluster.dir[1] = *init_dir_entry("..", 0x00, root);
    
    // creates empty dir entries
    for(int i = 2; i < CLUSTER/sizeof(dir_t); i++)
        cluster.dir[i] = *init_dir_entry("", 0x00, 0x0000);
    
    return cluster;
}


/**** get ****/
/** gets the location of the first empty cluster **/
uint16_t get_first_fat_empty(table_t* table){
    
    for(int i = BOOT+TABLE; i < FAT; i++)
        if(table->fat[i] == 0x0000)
            return (uint16_t)(i - (BOOT+TABLE));
    
    printf("Theres no more space in the system!!\n");
    return FALSE;
}

/** gets the location of 1st empty dir entry in curr dir **/
uint8_t get_first_dir_empty(cluster_t* current){
    
    for(int i = 0; i < CLUSTER/sizeof(dir_t); i++)
        if(current->dir[i].filename[0] == 0)
            return i;
    
    printf("Theres no more space in the directory!!\n");
    return FALSE;
}

/** gets a cluster from the system **/
cluster_t* get_cluster(int offset){
    
    FILE* file = fopen("fat.part", "r");
    
    cluster_t* cluster = init_cluster();
    
    offset += BOOT+TABLE;
    offset *= CLUSTER;
    
    // jumps boot, fat, offset
    fseek(file, offset, SEEK_SET);
    // read cluster
    fread(cluster, 1, sizeof(cluster_t), file);
    
    fclose(file);
    
    return cluster;
}

/** reads FAT table to memory **/
table_t* get_fat(){
    
    FILE* file = fopen("fat.part", "r");
    
    table_t* table = init_fat();
    
    // jumps boot
    fseek(file, BOOT*CLUSTER, SEEK_SET);
    // reads table
    fread(table, 1, TABLE*CLUSTER, file);
    
    fclose(file);
    
    return table;
}


/**** set ****/
/** overwrite FAT table in the system **/
void set_fat(table_t* table){
    
    FILE* file = fopen("fat.part", "r+");
    
    // jumps boot
    fseek(file, BOOT*CLUSTER, SEEK_SET);
    // saves table
    fwrite(table, 1, TABLE*CLUSTER, file);   
    
    fclose(file);
}

/** overwrites a cluster in the system **/
void set_cluster(int offset, cluster_t* cluster){
    
    offset += BOOT+TABLE;
    offset *= CLUSTER;
    
    FILE* file = fopen("fat.part", "r+");
    
    // jumps boot, table and offset
    fseek(file, offset, SEEK_SET);
    // saves table
    fwrite(cluster, 1, CLUSTER, file);
    
    fclose(file);
}
