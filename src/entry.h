#ifndef __ENTRY__
#define __ENTRY__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// command types
#define EXEC_TYPE 1
#define DIR_TYPE 2
#define DOC_TYPE 3
#define STR_TYPE 4
#define EXIT_TYPE 5

#define COMMAND_SIZE 10
#define PATH_SIZE 20
#define STRING_SIZE 100

typedef struct entry_t{
    int type;
    char* command;
    char* path;
    char* string;
} entry_t;

entry_t* new_entry();

entry_t* read_entry();

char* extract_command(char*, int*);
char* extract_string(char*, int*);
char* extract_path(char*, int*);

int verify_command(char*);

#endif
