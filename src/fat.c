#include <unistd.h>
#include "entry.h"

void get_entry(entry_t*);

int main(){
        
        entry_t* entry;
        entry = new_entry();
    
    do {

        get_entry(entry);
        
        printf("type: %d\n", entry->type);
        printf("command: %s\n", entry->command);
        printf("path: %s\n", entry->path);
        printf("string: %s\n", entry->string);
        
    } while (strcmp(entry->command, "exit"));
    
    return 0;
}

void get_entry(entry_t* entry){
    
    read(0, &entry->type, sizeof(int));
    read(0, entry->command, COMMAND_SIZE * sizeof(char));
    read(0, entry->path, STRING_SIZE * sizeof(char));
    read(0, entry->string, STRING_SIZE * sizeof(char));
    
}
