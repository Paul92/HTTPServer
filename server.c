#include<stdio.h>
#include<strings.h>
#include<stdlib.h>
#include<string.h>

#include<unistd.h>

#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>

#include "error.h"

#define PORT 80

int server(){

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd<0)
        errorOpeningSocket();
    
    struct sockaddr_in serv_addr, cli_addr;
    memset((char*) &serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    if(bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
        errorOnBinding();

    listen(sockfd, 5);

    socklen_t cilen = sizeof(cli_addr);

    while(1){
        int newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &cilen);
        if(newsockfd < 0)
            errorOnAccept();
        else{
            char data[1000];
            read(newsockfd, data, 1000);
            printf("%s\n", data);
        }
    }

    return 0;
}

int main(int argc, char** argv){
    server();
    return 0;
}
