#include "fat.h"

table_t* table;
cluster_t* current;

char file_path[50] = "fat.part";

void init(){
    
    FILE* file = fopen(file_path, "w");
    
    cluster_t* boot = init_boot();
    table = init_fat();
    cluster_t* cluster = init_cluster();
    cluster_t root = init_dir(0x0001, 0x0001);
    
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
        // load fat table and root dir
        table = get_fat(file);
        current = get_root(file);
        printf("FAT system loaded!!\n");
        // set flag fat_loaded, close file
        set_fat_loaded(TRUE);
        fclose(file);
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
    dir = *init_dir_entry(name, (uint16_t)fat_empty);
    current->dir[dir_empty] = dir;
    
    // creates dir cluster
    cluster_t cluster =
        init_dir(current->dir[0].first_block, fat_empty);
    
    FILE* file = fopen(file_path, "r+");
    
    int cur_dir = (int) (current->dir[0].first_block - 1);
    fseek(file, (BOOT+TABLE+cur_dir)*CLUSTER, SEEK_SET);
    fwrite(current, 1, CLUSTER, file);
    
    /* verificar se deu certo esse write */
    cur_dir = (int) (fat_empty - 1);
    fseek(file, (BOOT+TABLE+cur_dir)*CLUSTER, SEEK_SET);
    fwrite(&cluster, 1, CLUSTER, file);
    
    /* salvar o valor do dir na tabela e no arquivo */
    table->fat[BOOT+TABLE+cur_dir] = fat_empty;
    
    fseek(file, BOOT*CLUSTER, SEEK_SET);
    fwrite(table, 1, TABLE*CLUSTER, file);



    // testar se salvou o cluster
    // ta ok, usar pra fazer o cd
    fseek(file, (BOOT+TABLE+1)*CLUSTER, SEEK_SET);
    fread(&cluster, 1, CLUSTER, file);
    
    *current = cluster;
    
    
    fclose(file);
    
}





void ls(){
    
    char name[18];
    
    for(int i = 0; i < CLUSTER/sizeof(dir_t); i++){
        hextoc(current->dir[i].filename, name);
        if(strcmp(name, ""))
            printf("%d: %s \n", i, name);
    }
}
