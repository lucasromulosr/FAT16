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
    
    switch(opt){
        
        case EXEC_TYPE:
                        init();
                        break;
                        
        case DIR_TYPE:
                        break;
        case DOC_TYPE:
                        break;
        case STR_TYPE:
                        break;
        
    }
    
}
