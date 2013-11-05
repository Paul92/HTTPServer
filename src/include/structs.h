#ifndef STRUCTS_H
#define STRUCTS_H

#define MAX_REQUEST 256

struct headers{
    char *keys[MAX_REQUEST];
    char *values[MAX_REQUEST];
};

void initHeaders(struct headers*);
void setHeader(struct headers*, const char*, const char*);
char* getHeader(struct headers*, const char*);
void printHeaders(struct headers*);

struct responseHeaders{
    int code;
    char* codeName;
    char* location;
    char* contentType;
    char* charset;
    int fileSize;
};

#endif
