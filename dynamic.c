#include <stdio.h>
#include <string.h>
#include "structs.h"
#include "dynamic.h"

#define LOCATION "localhost"
#define RESPONSE_SIZE 1024
#define INF 214748364

char *patrat(char uri[MAX_REQUEST_LINE]){

    while(*uri!='?')
        uri++;

    uri++;  // to remove '?' too

    char *response = malloc(RESPONSE_SIZE);

    int val = INF;
    int ret = sscanf(uri, "a=%d", &val);

    if(ret == 0 || val == INF)
        strcpy(response, "<html>A avut loc o eroare</html>");
    else
        sprintf(response, "<html>Patratul lui %d este %d</html>", val, val*val);

    return response;
}


void dynamicHandler(struct HTTPRequest *request, int sockfd){
    struct responseHeaders headers = {200, "OK", LOCATION, "text/html", "UTF-8", 0};
    char *response;
    if(strncmp(request -> uri, "/patrat", 7) == 0){
        response = patrat(request -> uri);
    }
    headers.fileSize = strlen(response);
    sendHeaders(headers, sockfd);
    write(sockfd, response, headers.fileSize);

}


