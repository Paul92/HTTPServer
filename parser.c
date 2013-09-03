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

struct headers* parseHTTPRequest(char *buffer){

    struct headers* req = malloc(sizeof(struct headers));
    initHeaders(req);

    char key[1024], val[1024];

    sscanf(buffer, "%s", val); 
    setHeader(req, "Method:", val);
    buffer += strlen(val) + 1;
    sscanf(buffer, "%s", val); 
    setHeader(req, "URI:", val);
    buffer += strlen(val) + 1;
    sscanf(buffer, "%s", val); 
    setHeader(req, "HTTP-Version:", val);
    buffer += strlen(val);

    buffer = gotoNextLine(buffer);

    while(*buffer != '\0'){
        sscanf(buffer, "%s %[^\n]", key, val);
        setHeader(req, key, val);

        buffer = gotoNextLine(buffer);
    }

    return req;

}
