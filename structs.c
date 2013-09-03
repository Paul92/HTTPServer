#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "structs.h"

void initHeaders(struct headers *heads){
    heads -> keys[0] = NULL;
    heads -> values[0] = NULL;
}

int setHeader(struct headers *heads, const char *key, const char *val){
    int keySize = strlen(key);
    int valSize = strlen(val);

    int place = 0;
    while(heads -> keys[place] != NULL)
        place++;

    heads -> keys[place] = malloc(keySize);
    strcpy(heads -> keys[place], key);

    heads -> values[place] = malloc(valSize);
    strcpy(heads -> values[place], val);

    heads -> keys[place+1] = NULL;
    heads -> values[place+1] = NULL;
}

char* getHeader(struct headers *heads, const char *key){
    int i;
    for(i = 0; heads -> keys[i] != NULL && strcmp(heads -> keys[i], key); i++);
    return heads -> keys[i];
}

void printHeaders(struct headers *heads){
    int i = 0;

    while(heads -> keys[i] != NULL){
        printf("%d %s %s\n", i, heads -> keys[i], heads -> values[i]);
    }
}



