#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "structs.h"

#define MAX_URI_SIZE 256
#define MAX_REQUEST_LINE 256
#define DOC_ROOT "."
#define LOCATION "localhost"

void sendFile(FILE *f, int sockfd){

    char character;
    while(!feof(f)){
        fscanf(f, "%c", &character);
        if(!feof(f)){
            write(sockfd, &character, 1);
        }
    }
    close(sockfd);

}

void staticHandler(struct HTTPRequest *request, int sockfd){

    char filePath[MAX_URI_SIZE] = DOC_ROOT;
    strcat(filePath, request -> uri);
    
    struct responseHeaders headers = {200, "OK", LOCATION, "text/html", "UTF-8", 0};         //defaults

    FILE *f = NULL;

    if(access(filePath, F_OK) != 0){
        headers.code = 404;
        headers.codeName = "Not Found";
        strcpy(filePath, "./not_found");
    }else if(access(filePath, R_OK) != 0){
        headers.code = 403;
        headers.codeName = "Forbidden";
        strcpy(filePath, "./forbidden");
    }else if(strcmp(request -> uri, "/") == 0){
        f = popen("/bin/ls", "r");
        FILE *p = popen("ls | wc -c", "r");  //another pipe for finding size
        fscanf(p, "%d", &headers.fileSize);
        pclose(p);
    }else{
        f = fopen(filePath, "r");
        fseek(f, 0, SEEK_END);
        headers.fileSize = ftell(f);
        fseek(f, 0, SEEK_SET);
    }
    sendHeaders(headers, sockfd);
    sendFile(f, sockfd);
}
