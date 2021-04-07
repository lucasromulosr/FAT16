#include "entry.h"
#include "fat.h"

void execute(entry_t*, table_t*, cluster_t*);

int main(){
    
    entry_t* entry;
    entry = new_entry();
    
    table_t* table = init_fat();
    cluster_t* current = init_cluster();

    do {
        
        entry = read_entry();
        if(entry->type != EXIT_TYPE)
            execute(entry, table, current);
        
    } while (strcmp(entry->command,"exit"));
    
    return 0;
}

void execute(entry_t* entry, table_t* table, cluster_t* current){
    
    int opt = entry->type;
    char comm[COMMAND_SIZE];
    strcpy(comm, entry->command);
    
    switch(opt){
        
        case EXEC_TYPE:
            if(!strcmp(comm, "ls")) ls(current);
            if(!strcmp(comm, "init")) init();
            if(!strcmp(comm, "load")) load(table, current);
            break;
                        
        case DIR_TYPE:
            if(!strcmp(comm, "mkdir"))
                mkdir(entry->path, table, current);
                        break;
        case DOC_TYPE:
                        break;
        case STR_TYPE:
                        break;
        
    }
    
}
