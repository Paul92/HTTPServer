#ifndef STRUCTS_H
#define STRUCTS_H

#define MAX_REQUEST_LINE 256

struct HTTPRequest{
    char method[MAX_REQUEST_LINE];    //GET POST or HEAD
    char uri[MAX_REQUEST_LINE];
    char host[MAX_REQUEST_LINE];
    char userAgent[MAX_REQUEST_LINE];
};

struct responseHeaders{
    int code;
    char* codeName;
    char* location;
    char* contentType;
    char* charset;
    int fileSize;
};

#endif
