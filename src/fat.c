#include "fat.h"


void init(){
    
    FILE* file = fopen("fat.part", "w");
    
    fat_t fat;
    
    
    printf("FAT SIZE: %zu <---\n", sizeof(fat));

    
    fwrite(&fat, 1, sizeof(fat_t), file);
    
    fclose(file);
}
