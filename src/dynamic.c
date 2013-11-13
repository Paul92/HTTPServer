#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <unistd.h>
#include <wait.h>

#include "include/structs.h"
#include "include/static.h"
#include "include/dynamic.h"
#include "include/constants.h"

void dynamicHandler(struct headers *request, int sockfd){
    struct responseHeaders headers = {200, "OK", LOCATION, "text/html", "UTF-8", 0, false};
    sendHeaders(headers, sockfd);

    char *uri = getHeader(request, "URI:");

    char filePath[MAX_URI_SIZE] = DOC_ROOT;
    strcat(filePath, uri);
    char *end = strchr(filePath, '?');
    if(end) *end = '\0';

    char* GetParams = strchr(uri, '?');
    if(GetParams){
        GetParams++;
    }

    char* cmd = "/bin/php-cgi";
    char *argv[] = {cmd,  filePath, NULL};
    extern char **environ;

    setenv("QUERY_STRING", GetParams, 1);
    setenv("REQUEST_METHOD", "GET", 1);
    setenv("REDIRECT_STATUS", "200", 1);
    setenv("GATEWAY_INTERFACE", "GGI/1.1", 1);
    setenv("SERVER_PROTOCOL", "HTTP/1.0", 1);
    setenv("SCRIPT_FILENAME", filePath, 1);
    setenv("SCRIPT_NAME", filePath+strlen(DOC_ROOT), 1);
    setenv("REQUEST_URI", strchr(uri, '/'), 1);
    setenv("DOCUMENT_ROOT", DOC_ROOT, 1);

    dup2(sockfd, STDOUT_FILENO);
    if(execve(cmd, argv, environ) == -1)
        printf("Something went wrong\n");

}


