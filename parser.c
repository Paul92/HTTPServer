#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "structs.h"
#include "parser.h"

char* gotoNextLine(char *buffer){
    while(*buffer != '\n'){
        *buffer++;
    }
    while(*buffer == '\n' || *buffer == '\r')
        *buffer++;
    return buffer;
}

struct HTTPRequest* parseHTTPRequest(char *buffer){
    struct HTTPRequest* req = malloc(sizeof(struct HTTPRequest));
    sscanf(buffer, "%s %s", req->method, req->uri); 
    buffer += strlen(req -> method) + strlen(req -> uri);
    buffer = gotoNextLine(buffer);

    if(strncmp(buffer, "Host", strlen("Host")) == 0){
        sscanf(buffer + strlen("Host") + 2, "%[^\n]", req -> host);    //+2 for : and space after the name of the request header
        buffer = gotoNextLine(buffer);
    }

    if(strncmp(buffer, "User-Agent", strlen("User-Agent")) == 0){
        sscanf(buffer + strlen("User-Agent") + 2, "%[^\n]", req -> userAgent);
        buffer = gotoNextLine(buffer);
    }

    return req;

}
