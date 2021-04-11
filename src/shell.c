#include "entry.h"
#include "fat.h"

void execute(entry_t*);

int main(){
    
    entry_t* entry;
    entry = new_entry();

    do {
        
        entry = read_entry();
        if(entry->type != EXIT_TYPE)
            execute(entry);
        
    } while (strcmp(entry->command,"exit"));
    
    return 0;
}

void execute(entry_t* entry){
    
    int opt = entry->type;
    char comm[COMMAND_SIZE];
    strcpy(comm, entry->command);
    
    switch(opt){
        
        case EXEC_TYPE:
                        if(!strcmp(comm, "ls")) ls();
                        if(!strcmp(comm, "init")) init();
                        if(!strcmp(comm, "load")) load();
                        if(!strcmp(comm, "help")) help();
                        break;
                        
                        
        case DIR_TYPE:
                        if(!strcmp(comm, "mkdir"))
                            mkdir(entry->path);
                        break;
                        
                        
        case DOC_TYPE:
                        if(!strcmp(comm, "create"))
                            create(entry->path);
                        if(!strcmp(comm, "unlink"))
                            unlink(entry->path);
                        if(!strcmp(comm, "read"))
                            read(entry->path);
                        break;
                        
                        
        case STR_TYPE:
                        if(!strcmp(comm, "write"))
                            write(entry->path, entry->string);
                        if(!strcmp(comm, "append"))
                            append(entry->path, entry->string);
                        break;
    }
}
