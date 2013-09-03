#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "structs.h"
#include "parser.h"
#include "static.h"
#include "dynamic.h"

#define PORT 80

#if 1
# define ERR_LOG(fmt, args...) fprintf(stderr, "%s (%d) in %s(): ", \
                __FILE__, __LINE__, __FUNCTION__); \
        fprintf(stderr, fmt, ## args)
#else
# define ERR_LOG(fmt, args...)
#endif

#define REQUEST_MAX_SIZE 8192

void sendHeaders(struct responseHeaders headers, int sockfd){

    char responseHeader[1000];

    sprintf(responseHeader, "HTTP/1.0 %d %s\n", headers.code, headers.codeName); 
    sprintf(responseHeader + strlen(responseHeader), "Location: %s\n", headers.location);
    sprintf(responseHeader + strlen(responseHeader), "Content-type: %s; charset=%s\n", headers.contentType, headers.charset);
    sprintf(responseHeader + strlen(responseHeader), "Content-Length: %d\n\n", headers.fileSize);

    write(sockfd, responseHeader, strlen(responseHeader));

}

void requestHandler(struct headers *request, int sockfd){
    char *uri = getHeader(request, "URI:");

    if(strncmp(uri, "/patrat", 7) == 0 || strcmp(uri, "/login") == 0 || strcmp(uri, "/verifica") == 0){
        printf("It's a dynamic request\n");
        dynamicHandler(request, sockfd);
    }else{
        printf("It's a static request\n");
        staticHandler(request, sockfd);
    }
}

int server(){

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd<0){
        ERR_LOG("Error creating socket");
    }
    
    struct sockaddr_in serv_addr, cli_addr;
    memset((char*) &serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    if(bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0){
        ERR_LOG("Error on binding");
    }

    listen(sockfd, 5);

    socklen_t cilen = sizeof(cli_addr);

    while(1){
        int newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &cilen);
        if(newsockfd < 0){
            ERR_LOG("Error on accept");
        }else{
            char request[REQUEST_MAX_SIZE];
            read(newsockfd, request, REQUEST_MAX_SIZE);
            printf("New request\n");
            struct headers *requestStruct;
            printf("Parsing...\n");
            requestStruct = parseHTTPRequest(request);
            printf("Parsed\n");
            requestHandler(requestStruct, newsockfd);
            printf("Handled. Wainting for a new one\n");
        }
    }

    return 0;
}

int main(int argc, char** argv){
    server();
    return 0;
}
