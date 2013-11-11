#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "include/structs.h"
#include "include/constants.h"

void sendFile(FILE *f, int sockfd){

    printf("So we're sending file....\n");
    char character;
    while(!feof(f)){
        fscanf(f, "%c", &character);
        if(!feof(f)){
            write(sockfd, &character, 1);
        }
    }

}

void sendHeaders(struct responseHeaders headers, int sockfd){

    char responseHeader[1000];

    sprintf(responseHeader, "HTTP/1.0 %d %s\n", headers.code, headers.codeName); 
    sprintf(responseHeader + strlen(responseHeader), "Location: %s\n", headers.location);
    sprintf(responseHeader + strlen(responseHeader), "Content-type: %s; charset=%s\n", headers.contentType, headers.charset);
    if(headers.sendLength)
        sprintf(responseHeader + strlen(responseHeader), "Content-Length: %d\n\n", headers.fileSize);

    write(sockfd, responseHeader, strlen(responseHeader));

}

void staticHandler(struct headers *request, int sockfd){

    char filePath[MAX_URI_SIZE] = DOC_ROOT;
    char *uri = getHeader(request, "URI:");
    strcat(filePath, uri);

    struct responseHeaders headers = {200, "OK", LOCATION, "text/html", "UTF-8", 0, false};         //defaults

    FILE *f = NULL;

    if(access(filePath, F_OK) != 0){
        headers.code = 404;
        headers.codeName = "Not Found";
        strcpy(filePath, "./not_found");
    }else if(access(filePath, R_OK) != 0){
        headers.code = 403;
        headers.codeName = "Forbidden";
        strcpy(filePath, "./forbidden");
    }else if(strcmp(uri, "/") == 0){
        f = popen("/bin/ls", "r");
        FILE *p = popen("ls | wc -c", "r");  //another pipe for finding size
        fscanf(p, "%d", &headers.fileSize);
        pclose(p);
    }
    printf("Opening file %s\n", filePath);
    if(f == NULL){
        f = fopen(filePath, "r");
        fseek(f, 0, SEEK_END);
        headers.fileSize = ftell(f);
        fseek(f, 0, SEEK_SET);
    }

    printf("Sending headers...\n");
    sendHeaders(headers, sockfd);
    printf("Sending file: %s\n", filePath);
    sendFile(f, sockfd);
}
