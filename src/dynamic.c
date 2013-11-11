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
    printf("URI %s\n", uri);

    char filePath[MAX_URI_SIZE] = DOC_ROOT;
    strcat(filePath, uri);
    char *end = strchr(filePath, '?');
    if(end) *end = '\0';

    char* bias = strchr(uri, '?');
    if(bias){
        bias++;
    }

    char* cmd = "/bin/php-cgi";
    char *argv[] = {cmd, "-f", filePath, NULL};
    extern char **environ;

    if(bias)
        setenv("QUERY_STRINGS", bias, 1);
    dup2(sockfd, STDOUT_FILENO);
    execve(cmd, argv, environ);

}


