#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

#define COMMAND_SIZE 10
#define STRING_SIZE 50
#define INPUT_SIZE 100

typedef struct entry_t{
    char* command;
    char* path;
    char* string;
} entry_t;

// le entradas ate que seja uma valida
entry_t* read_entry();

char* extract_command(char*, int*);
char* extract_string(char*, int*);
char* extract_path(char*, int*);
int verify_command(char*);


int main(){

    char* args[] = {"./manager", NULL}; // execv
    int fd[2];      // pipe
    pid_t pid;     // pid
    
    entry_t* entry;

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
        
//         execv(args[0], args);
        
    } else {    // pai
        
        close(fd[0]);
        
        do {
            entry = read_entry();
            printf("command: %s <--\n", entry->command);
            printf("path: %s <--\n", entry->path);
            printf("string: %s <--\n", entry->string);
//             write(fd[1], entry, 1);
        }while (strcmp(entry->command,"exit"));
        
        close(fd[1]);
    }

    return 0;
}

entry_t* read_entry(){

    char input[INPUT_SIZE];
//     char aux[STRING_SIZE];
    
    entry_t* entry;
    entry = (entry_t*) malloc (sizeof(entry_t));
    entry->command = (char*) malloc (COMMAND_SIZE * sizeof(char));
    entry->path = (char*) malloc (STRING_SIZE * sizeof(char));
    entry->string = (char*) malloc (STRING_SIZE * sizeof(char));
    
    int type;   // command type // explicar os tipos
    do{
        fgets(input, INPUT_SIZE, stdin);
        printf("%ld %s", strlen(input), input);
        
        int index;     // index in the input
        index = 0;
        
        entry->command = extract_command(input, &index);
        
        type = verify_command(entry->command);
        if(type){
            
            switch(type){
                case 1:
                        return entry;
                case 2:
                        entry->path = extract_path(input, &index);
                        return entry;
                case 3:
                        entry->path = extract_path(input, &index);
                        return entry;
                case 4: 
                        entry->string = extract_string(input, &index);  // fazer tratamento de ""
                        entry->path = extract_path(input, &index);
                        return entry;
            }
        }
        else
            printf("Invalid command!!\n");
        
    }while(!type);
}

int verify_command(char* command){
    
    int type = 0;   // command type
    int var;
    
    var = strcmp(command, "exit") * strcmp(command, "init");
    var *= strcmp(command, "load");
    if(!var) return 1;
    
    var = strcmp(command, "ls") * strcmp(command, "mkdir");
    if(!var) return 2;
    
    var = strcmp(command, "create") * strcmp(command, "unlink");
    var *= strcmp(command, "read");
    if(!var) return 3;
    
    var = strcmp(command, "write") * strcmp(command, "append");
    if(!var) return 4;
    
    return 0;
}

char* extract_command(char* input, int* i){
    
    char* command;
    command = (char*) malloc (COMMAND_SIZE * sizeof(char));
    
    while(input[*i] != ' ' && input[*i] != '\n'){
        command[*i] = input[*i];
        command[++(*i)] = '\0';
    }
    
    return command;
}


char* extract_path(char* input, int* i){
    
    char* path;
    path = (char*) malloc (STRING_SIZE * sizeof(char));
    
    int index = 0;  // path index
    
    (*i)++;
    
    while(input[*i] != '\n'){
        path[index++] = input[(*i)++];
        path[index] = '\0';
    }
    
    return path;
}


char* extract_string(char* input, int* i){
    
    char* string;
    string = (char*) malloc (STRING_SIZE * sizeof(char));
    
    int index = 0;  // string index
    
    (*i)+= 2;
    
    while(input[*i] != '"'){
        string[index++] = input[(*i)++];
        string[index] = '\0';
    }
    
    (*i)++;
    
    return string;
}










