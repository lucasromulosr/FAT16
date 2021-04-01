#include "entry.h"

entry_t* new_entry(){
    
    entry_t* entry;
    entry = (entry_t*) malloc (sizeof(entry_t));
    entry->command = (char*) malloc (COMMAND_SIZE * sizeof(char));
    entry->path = (char*) malloc (STRING_SIZE * sizeof(char));
    entry->string = (char*) malloc (STRING_SIZE * sizeof(char));
        
    entry->type = 0;
    strcpy(entry->command, "\0");
    strcpy(entry->path, "\0");
    strcpy(entry->string, "\0");
    
    return entry;
}

entry_t* read_entry(){

    char input[INPUT_SIZE];
    
    entry_t* entry;
    entry = new_entry();
    
    int type;   // command type // explicar os tipos
    do {
        fgets(input, INPUT_SIZE, stdin);
        
        int index;     // index in the input
        index = 0;
        
        entry->command = extract_command(input, &index);
        
        type = verify_command(entry->command);
        if(type){
            
            entry->type = type;
            
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
                        entry->string = extract_string(input, &index);
                        
                        if(!strcmp(entry->string, "")){
                            type = 0;
                            printf("Wrong string format!!\n");
                        }
                        else{
                            entry->path = extract_path(input, &index);
                            return entry;
                        }
            }
        }
        else
            printf("Invalid command!!\n");
        
    } while (!type);
}

int verify_command(char* command){
    
    int type;
    
    type = strcmp(command, "exit") * strcmp(command, "init");
    type *= strcmp(command, "load");
    if(!type) return 1;
    
    type = strcmp(command, "ls") * strcmp(command, "mkdir");
    if(!type) return 2;
    
    type = strcmp(command, "create") * strcmp(command, "unlink");
    type *= strcmp(command, "read");
    if(!type) return 3;
    
    type = strcmp(command, "write") * strcmp(command, "append");
    if(!type) return 4;
    
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
    
    if(input[++(*i)] != '"')
        return "";
    (*i)++;
    
    while(input[*i] != '"'){
        
        if(input[*i] == '\n')
            return "";
        
        string[index++] = input[(*i)++];
        string[index] = '\0';
    }
    
    (*i)++;
    
    return string;
}
