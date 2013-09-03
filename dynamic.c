#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "structs.h"
#include "dynamic.h"

#define LOCATION "localhost"
#define RESPONSE_SIZE 1024
#define INF 214748364

char *patrat(char* uri){

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


void dynamicHandler(struct headers *request, int sockfd){
    struct responseHeaders headers = {200, "OK", LOCATION, "text/html", "UTF-8", 0};
    char *response;
    char *uri = getHeader(request, "URI");
    if(strncmp(uri, "/patrat", 7) == 0){
        response = patrat(uri);
    }
    headers.fileSize = strlen(response);
    sendHeaders(headers, sockfd);
    write(sockfd, response, headers.fileSize);

}


