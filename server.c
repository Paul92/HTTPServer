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

#if 1
# define ERR_LOG(fmt, args...) fprintf(stderr, "%s (%d) in %s(): ", \
                __FILE__, __LINE__, __FUNCTION__); \
        fprintf(stderr, fmt, ## args)
#else
# define ERR_LOG(fmt, args...)
#endif

#define DOC_ROOT "."
#define LOCATION "localhost"

struct headerValue{
    int code;
    char* codeName;
    char* location;
    char* contentType;
    char* charset;
    int fileSize;
};

void sendHeaders(struct headerValue headers, int sockfd){

    char responseHeader[1000];

    sprintf(responseHeader, "HTTP/1.0 %d %s\n", headers.code, headers.codeName); 
    sprintf(responseHeader + strlen(responseHeader), "Location: %s\n", headers.location);
    sprintf(responseHeader + strlen(responseHeader), "Content-type: %s; charset=%s\n", headers.contentType, headers.charset);
    sprintf(responseHeader + strlen(responseHeader), "Content-Length: %d\n\n", headers.fileSize);

    write(sockfd, responseHeader, strlen(responseHeader));

}

void sendFile(FILE *f, int sockfd){

    char character;
    while(!feof(f)){
        fscanf(f, "%c", &character);
        if(!feof(f)){
            write(sockfd, &character, 1);
        }
    }
    close(sockfd);

}

void staticHandler(char file[100], int sockfd){

    char filePath[100] = DOC_ROOT;
    strcat(filePath, file);
    
    struct headerValue headers;         //defaults
    headers.code = 200;
    headers.codeName = "OK";
    headers.location = LOCATION;
    headers.contentType = "text/html";
    headers.charset = "UTF-8";

    FILE *f = NULL;

    if(access(filePath, F_OK) != 0){
        headers.code = 404;
        headers.codeName = "Not Found";
        strcpy(filePath, "./not_found");
    }else if(access(filePath, R_OK) != 0){
        headers.code = 403;
        headers.codeName = "Forbidden";
        strcpy(filePath, "./forbidden");
    }else if(strcmp(file, "/") == 0){
        f = popen("/bin/ls", "r");
        FILE *p = popen("ls | wc -c", "r");  //another pipe for finding size
        fscanf(p, "%d", &headers.fileSize);
        pclose(p);
    }else{
        f = fopen(filePath, "r");
        fseek(f, 0, SEEK_END);
        headers.fileSize = ftell(f);
        fseek(f, 0, SEEK_SET);
    }
    sendHeaders(headers, sockfd);
    sendFile(f, sockfd);
}

void parse(char request[1000], int sockfd){
    char data[100];
    sscanf(request, "%s", data);
    int n = strlen(data) + 1;
    if(strcmp(data, "GET") == 0){
        char file[100];
        sscanf(request + n, "%s", file);
        n += strlen(file) + 1;
        if(strcmp(file, "/patrat") == 0){

        }else if(strcmp(file, "/login") == 0){

        }else if(strcmp(file, "/verifica") == 0){

        }else{
            staticHandler(file, sockfd);
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
        printf("ended\n");
    }

    return 0;
}

int main(int argc, char** argv){
    server();
    return 0;
}
