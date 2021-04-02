#include "entry.h"

int main(){
    
    entry_t* entry;
    entry = new_entry();

    do {
        
        entry = read_entry();
        // execute entry
        
    } while (strcmp(entry->command,"exit"));
    
    return 0;
}
