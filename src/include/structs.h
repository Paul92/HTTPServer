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
    int sendLength;
};

#define false 0
#define true 1

#endif
