#include<stdio.h>
#include<strings.h>
#include<stdlib.h>
#include<string.h>

#include<unistd.h>

#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>

#define PORT 80

# define ERR_LOG(fmt, args...) fprintf(stderr, "%s (%d) in %s(): ", \
                __FILE__, __LINE__, __FUNCTION__); \
        fprintf(stderr, fmt, ## args)

#define DOC_ROOT "."
#define LOCATION "localhost"

void sendFile(char file[100], int sockfd){

    char filePath[100] = DOC_ROOT;
    strcat(filePath, file);
    
    int statusCode = 200, fileSize;
    char* statusName = "OK";
    FILE *f = NULL;

    char responseHeader[1000];
    if(access(filePath, F_OK) != 0){
        statusCode = 404;
        statusName = "Not Found";
        strcpy(filePath, "./not_found");
    }else if(access(filePath, R_OK) != 0){
        statusCode = 403;
        statusName = "Forbidden";
        strcpy(filePath, "./forbidden");
    }
    printf("Status: %d\n", statusCode);

    f = fopen(filePath, "r");

    fseek(f, 0, SEEK_END);
    fileSize = ftell(f);
    fseek(f, 0, SEEK_SET);

    sprintf(responseHeader, "HTTP/1.0 %d %s\n", statusCode, statusName); 
    sprintf(responseHeader + strlen(responseHeader), "Location: %s\n", LOCATION);
    sprintf(responseHeader + strlen(responseHeader), "Content-type: text/html; charset=UTF-8\n");
    sprintf(responseHeader + strlen(responseHeader), "Content-Length: %d\n\n", fileSize);

    write(sockfd, responseHeader, strlen(responseHeader));

    printf("Sending file %s\n", filePath);
    printf("FILE %p\n", f);
    char character;
    while(!feof(f)){
        fscanf(f, "%c", &character);
        if(!feof(f)){
            write(sockfd, &character, 1);
        }
    }
    close(sockfd);

}


void parse(char request[1000], int sockfd){
    char data[100];
    sscanf(request, "%s", data);
    int n = strlen(data) + 1;
    printf("%d\n", n);
    if(strcmp(data, "GET") == 0){
        char file[100];
        sscanf(request + n, "%s", file);
        n += strlen(file) + 1;
        if(strcmp(file, "/patrat") == 0){

        }else if(strcmp(file, "/login") == 0){

        }else if(strcmp(file, "/verifica") == 0){

        }else{
            sendFile(file, sockfd);
        }
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
            char request[1000];
            int requestIndex=0;
            char data;
            int ok=1;
            while(ok){
                read(newsockfd, &data, 1);
                request[requestIndex++] = data;
                if(data == '\n'){
                    parse(request, newsockfd);
                    requestIndex=0;
                    ok=0;
                }

            }
            
        }
    }

    return 0;
}

int main(int argc, char** argv){
    server();
    return 0;
}