#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "structs.h"
#include "static.h"
#include "dynamic.h"
#include "constants.h"

void dynamicHandler(struct headers *request, int sockfd){
    struct responseHeaders headers = {200, "OK", LOCATION, "text/html", "UTF-8", 0};
    char *response;
    char *uri = getHeader(request, "URI:");

    char filePath[MAX_URI_SIZE] = DOC_ROOT;
    strcat(filePath, uri);

    char *end = strchr(filePath, '?');
    if(end) *end = '\0';

    char* bias = strchr(uri, '?');
    if(bias) bias++;

    printf("%s\n", bias);
    char* space;
    while(bias && (space = strchr(bias, '&'))){
        *space = ' ';
    }

    printf("%s\n", bias);
    printf("CMD\n");

    char cmd[MAX_CLI_COMMAND] = "php-cgi -f ";
    strcat(cmd, filePath);
    strcat(cmd, " ");
    strcat(cmd, bias);

    printf("CMD: %s", cmd);

    FILE *f = popen(cmd, "r");
    if(f != NULL){
        fseek(f, 0, SEEK_END);
        headers.fileSize = ftell(f);
        fseek(f, 0, SEEK_SET);
    }

    sendHeaders(headers, sockfd);

    sendFile(f, sockfd);


}


