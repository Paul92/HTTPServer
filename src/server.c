#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "include/structs.h"
#include "include/parser.h"
#include "include/static.h"
#include "include/dynamic.h"

#define PORT 80

#if 1
# define ERR_LOG(fmt, args...) fprintf(stderr, "%s (%d) in %s(): ", \
                __FILE__, __LINE__, __FUNCTION__); \
        fprintf(stderr, fmt, ## args)
#else
# define ERR_LOG(fmt, args...)
#endif

#define REQUEST_MAX_SIZE 8192

void shutdownDaemon(){
    exit(1);
}

void SIGCHLD_handler(int sig){

    pid_t pid;

    while((pid = waitpid(-1, NULL, 0)) > 0){
        printf("Reaped %d\n", pid);
    }
    if(errno != ECHILD){
        ERR_LOG("waitpid error\n");
        shutdownDaemon();
    }

}

void SIGINT_handler(int sig){

    ERR_LOG("Shutdown command\n");

    pid_t groupPid = getpgrp();
    kill((-1)*groupPid, SIGTERM);
}

void requestHandler(struct headers *request, int sockfd){
    char *uri = getHeader(request, "URI:");

    if(strstr(uri, ".php")){
        dynamicHandler(request, sockfd);
    }else{
        staticHandler(request, sockfd);
    }
}

int server(){

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd<0){
        ERR_LOG("Error creating socket");
        shutdownDaemon();
    }
    
    struct sockaddr_in serv_addr, cli_addr;
    memset((char*) &serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    if(bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0){
        ERR_LOG("Error on binding");
        shutdownDaemon();
    }

    listen(sockfd, 5);

    socklen_t cilen = sizeof(cli_addr);

    while(1){
        int newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &cilen);
        if(newsockfd < 0){
            ERR_LOG("Error on accept");
            shutdownDaemon();
        }else{
            pid_t pid = fork();
            if(pid < 0){
                ERR_LOG("Error on fork\n");
                shutdownDaemon();
            }
            if(pid == 0){
                char request[REQUEST_MAX_SIZE];
                read(newsockfd, request, REQUEST_MAX_SIZE);
                struct headers *requestStruct;
                requestStruct = parseHTTPRequest(request);
                requestHandler(requestStruct, newsockfd);
                exit(0);
            }
            close(newsockfd);
        }
    }

    return 0;
}

int main(int argc, char** argv){
    if(signal(SIGCHLD, SIGCHLD_handler) == SIG_ERR || 
       signal(SIGINT,  SIGINT_handler ) == SIG_ERR){
        ERR_LOG("Signal error\n");
        shutdownDaemon(1);
    }
    server();
    return 0;
}
