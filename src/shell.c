#include <unistd.h>
#include "entry.h"

int main(){

    char* args[] = {"./fat", NULL}; // execv
    int fd[2];      // pipe
    pid_t pid;     // pid
    
    entry_t* entry;
    entry = new_entry();

    if(pipe(fd) < 0){
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    if((pid = fork()) == -1){
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if(pid == 0){   // filho
        
        close(fd[1]);
        
        dup2(fd[0], STDIN_FILENO);
        
        close(fd[0]);
        
        execv(args[0], args);
        
    } else {    // pai
        
        close(fd[0]);
        
        do {
            
            entry = read_entry();

            write(fd[1], &entry->type, sizeof(int));         
            write(fd[1], entry->command, COMMAND_SIZE * sizeof(char));
            write(fd[1], entry->path, STRING_SIZE * sizeof(char));
            write(fd[1], entry->string, STRING_SIZE * sizeof(char));
            
        } while (strcmp(entry->command,"exit"));
        
        close(fd[1]);
    }

    return 0;
}
