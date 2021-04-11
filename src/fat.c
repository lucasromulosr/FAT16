#include "fat.h"

table_t* table;
cluster_t* current;

char file_path[50] = "fat.part";

/** list dir contents **/
void ls(){
    
    // check flag fat loaded
    if(!check_sys_load()) return;
    
    char name[23];
    
    for(int i = 0; i < CLUSTER/sizeof(dir_t); i++){
        hextoc(18, current->dir[i].filename, name);
        if(strcmp(name, "")){
            if(current->dir[i].attributes == 0x01)
                strcat(name, ".txt");
            printf("%s \t", name);
        }
    }
    
    printf("\n");
}

/** init a new FAT system **/
// wipes all the previously data
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

/** loads FAT table and / **/
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

/** creates a new dir in the current dir **/
void mkdir(char* name){
    
    // check flag fat loaded
    if(!check_sys_load()) return;
    
    // finds 1st empty cluster
    uint16_t fat_empty = get_first_fat_empty(table);
    if(!fat_empty)
        return;
    
    // finds 1st empty dir entry
    uint8_t dir_empty = get_first_dir_empty(current);
    if(!dir_empty)
        return;
    
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
    uint16_t current_first_block = current->dir[0].first_block;
    int current_pos = 
        current_first_block == 0xffff ? 0 : current_first_block;
    set_cluster((int)current_pos, current);
}

/** creates a new file in the curr dir **/
void create(char* name){
    
    // check flag fat loaded
    if(!check_sys_load()) return;
    
    // finds 1st empty cluster
    uint16_t fat_empty = get_first_fat_empty(table);
    if(!fat_empty)
        return;
    
    // finds 1st empty dir entry
    uint8_t dir_empty = get_first_dir_empty(current);
    if(!dir_empty)
        return;
    
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
    uint16_t current_first_block = current->dir[0].first_block;
    int current_pos = 
        current_first_block == 0xffff ? 0 : current_first_block;
    set_cluster((int)current_pos, current);
}

/** unlink(rm) a file/dir from the curr dir **/
void unlink(char* name){
    
    // check flag fat loaded
    if(!check_sys_load()) return;
    
    char filename[18];
    int flag = FALSE;
    
    // goes through dirs and files
    for(int i = 2; i < (CLUSTER/sizeof(dir_t)) -2; i++){
        
        // find something w/ a corresponding name
        hextoc(18, current->dir[i].filename, filename);
        if(!strcmp(filename, name)){
            
            // if dir, checks if its empty
            if(current->dir[i].attributes == 0x00 &&
                current->dir[i].size > 0x0000){
                
                printf("Dir is not empty!!\n");
                return;
            }
            
            // gets offset in fat
            // position in system: BOOT+TABLE+offset
            int offset = (int)current->dir[i].first_block;
            
            // saves an empty cluster (flush)(system)
            cluster_t cluster = *init_cluster();
            set_cluster(offset, &cluster);
            
            // flush in fat table
            table->fat[BOOT+TABLE+offset] = 0x0000;

            // flush in the curr dir
            dir_t dir = *init_dir_entry("", 0x00, 0x0000);
            current->dir[i] = dir;
            current->dir[0].size -= 0x0001;
            
            flag = TRUE;
        }
    }
    
    // saves curr dir into the systema
    uint16_t current_first_block = current->dir[0].first_block;
    int current_pos = 
        current_first_block == 0xffff ? 0 : current_first_block;
    set_cluster(current_pos, current);
    
    // saves fat table in the system
    set_fat(table);
    
    if(flag) printf("Unlinked!!\n");
    else printf("Nothing to unlike!!\n");
}

/** prints the contents of a file **/
void read(char* path){
    
    // check flag fat loaded
    if(!check_sys_load()) return;
    
    char filename[18];
    char texto[CLUSTER];
    
    for(int i = 2; i < (int)current->dir[0].size + 2; i++){
        
        hextoc(18, current->dir[i].filename, filename);
        
        // checks if its a dir
        if(!strcmp(path, filename) && !current->dir[i].attributes){
            printf("Cannot read dir!!\n");
            return;
        }
        
        // checks if its the file
        if(!strcmp(path, filename) && current->dir[i].attributes){
        
            int offset = current->dir[i].first_block;
            
            // gets cluster, convert, print
            cluster_t cluster = *get_cluster(offset);
            hextoc(CLUSTER, cluster.data, texto);
            printf("%s\n", texto);
        }
    }
}

/** overwrites a file **/
void write(char* path, char* string){
    
    // check flag fat loaded
    if(!check_sys_load()) return;
    
    char filename[18];
    uint8_t data[100];

    for(int i = 2; i < (int)current->dir[0].size + 2; i++){
        
        hextoc(18, current->dir[i].filename, filename);
        
        // checks if its a dir
        if(!strcmp(path, filename) && !current->dir[i].attributes){
            printf("Cannot write dir!!\n");
            return;
        }
        
        // checks if its the file
        if(!strcmp(path, filename) && current->dir[i].attributes){
            
            ctohex(100, string, data);
            
            int offset = (int)current->dir[i].first_block;
            
            // set file size
            current->dir[i].size = (uint32_t)strlen(string);
            
            // creates new empty cluster, writes, saves
            cluster_t cluster = *init_cluster();
            for(int i = 0; i < strlen(string); i++)
                cluster.data[i] = data[i];
            set_cluster(offset, &cluster);

        }
    }
        
    // saves curr dir into the system
    uint16_t current_first_block = current->dir[0].first_block;
    int current_pos = 
        current_first_block == 0xffff ? 0 : current_first_block;
    set_cluster(current_pos, current);
    
    printf("Doc writed!!\n");
}

/** append contents to file **/
void append(char* path, char* string){
    
    // check flag fat loaded
    if(!check_sys_load()) return;
    
    char filename[18];
    uint8_t data[100];

    for(int i = 2; i < (int)current->dir[0].size + 2; i++){
        
        hextoc(18, current->dir[i].filename, filename);
        
        // checks if its a dir
        if(!strcmp(path, filename) && !current->dir[i].attributes){
            printf("Cannot append dir!!\n");
            return;
        }
        
        // checks if its the fle
        if(!strcmp(path, filename) && current->dir[i].attributes){
            
            ctohex(100, string, data);
            
            int offset = (int)current->dir[i].first_block;
            
            // set file size
            int file_size = (int)current->dir[i].size;
            int size = file_size + strlen(string);
            current->dir[i].size = (uint32_t)size;

            // creates new empty cluster, writes, saves
            cluster_t cluster = *get_cluster(offset);
            for(int i = 0; i < strlen(string); i++)
                cluster.data[file_size+i] = data[i];
            set_cluster(offset, &cluster);
        }
    }
        
    // saves curr dir into the system
    uint16_t current_first_block = current->dir[0].first_block;
    int current_pos = 
        current_first_block == 0xffff ? 0 : current_first_block;
    set_cluster(current_pos, current);
    
    printf("Doc writed!!\n");
}

void help(){
    printf("| ----------------------- | ------------------------------------------------- |\n");
    printf("| Comando                 | Descrição                                         |\n");
    printf("| ----------------------- | ------------------------------------------------- |\n");
    printf("| help                    | Imprime ajuda                                     |\n");
    printf("| ----------------------- | ------------------------------------------------- |\n");
    printf("| init                    | Inicializar o sistema de arquivos                 |\n");
    printf("| ----------------------- | ------------------------------------------------- |\n");
    printf("| load                    | Carregar o sistema de arquivos do disco           |\n");
    printf("| ----------------------- | ------------------------------------------------- |\n");
    printf("| ls                      | Lista diretório                                   |\n");
    printf("| ----------------------- | ------------------------------------------------- |\n");
    printf("| mkdir                   | Cria diretório                                    |\n");
    printf("| ----------------------- | ------------------------------------------------- |\n");
    printf("| creat                   | Cria arquivo                                      |\n");
    printf("| ----------------------- | ------------------------------------------------- |\n");
    printf("| unlink                  | Excluir arquivo ou diretório                      |\n");
    printf("| ----------------------- | ------------------------------------------------- |\n");
    printf("| write \"string\"          | Escrever dados em um arquivo                      |\n");
    printf("| ----------------------- | ------------------------------------------------- |\n");
    printf("| append \"string\"         | Anexar dados em um arquivo                        |\n");
    printf("| ----------------------- | ------------------------------------------------- |\n");
    printf("| read                    | Lê o conteúdo de um arquivo                       |\n");
    printf("| ----------------------- | ------------------------------------------------- |\n");
    printf("| exit                    | Sai do simulador                                  |\n");
    printf("| ----------------------- | ------------------------------------------------- |\n");
}
