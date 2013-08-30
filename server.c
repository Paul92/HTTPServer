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
#define REQUEST_MAX_SIZE 8192
#define MAX_URI_SIZE 255

struct HTTPRequest{
    char *method;    //GET POST or HEAD
    char *uri;
    char *authorization;
    char *from;
    char *ifModifiedSince;
    char *referer;
    char *userAgent;
};

struct responseHeaders{
    int code;
    char* codeName;
    char* location;
    char* contentType;
    char* charset;
    int fileSize;
};


void sendHeaders(struct responseHeaders headers, int sockfd){

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

void staticHandler(struct HTTPRequest *request, int sockfd){

    char filePath[MAX_URI_SIZE] = DOC_ROOT;
    strcat(filePath, request -> uri);
    
    struct responseHeaders headers = {200, "OK", LOCATION, "text/html", "UTF-8", 0};         //defaults

    FILE *f = NULL;

    if(access(filePath, F_OK) != 0){
        headers.code = 404;
        headers.codeName = "Not Found";
        strcpy(filePath, "./not_found");
    }else if(access(filePath, R_OK) != 0){
        headers.code = 403;
        headers.codeName = "Forbidden";
        strcpy(filePath, "./forbidden");
    }else if(strcmp(request -> uri, "/") == 0){
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

void dynamicHandler(struct HTTPRequest *request, int sockfd){
    printf("CRAP");
}

void gotoNextLine(char *buffer){
    while(*buffer != '\n')
        buffer++;
    while(*buffer == '\n' || *buffer == '\r')
        buffer++;
}

struct HTTPRequest* parseHTTPRequest(char *buffer){
    struct HTTPRequest* req = malloc(sizeof(struct HTTPRequest));
    sscanf(buffer, "%s %s", req->method, req->uri); 
    buffer += strlen(req -> method) + strlen(req -> uri);
    gotoNextLine(buffer);

    if(strncmp(buffer, "Authorization", strlen("Authorization")) == 0){
        sscanf(buffer + strlen("Authorization") + 2, "%s", req -> authorization);
        gotoNextLine(buffer);
    }
    if(strncmp(buffer, "From", strlen("From")) == 0){
        sscanf(buffer + strlen("From") + 2, "%s", req -> from);
        gotoNextLine(buffer);
    }
    if(strncmp(buffer, "If-Modified-Since", strlen("If-Modified-Since")) == 0){
        sscanf(buffer + strlen("If-Modified-Since") + 2, "%s", req -> ifModifiedSince);
        gotoNextLine(buffer);
    }
    if(strncmp(buffer, "Referer", strlen("Referer")) == 0){
        sscanf(buffer + strlen("Referer") + 2, "%s", req -> referer);
        gotoNextLine(buffer);
    }
    if(strncmp(buffer, "User-Agent", strlen("User-Agent")) == 0){
        sscanf(buffer + strlen("User-Agent") + 2, "%s", req -> userAgent);
        gotoNextLine(buffer);
    }

    return req;

}

void requestHandler(struct HTTPRequest *request, int sockfd){
    if(strcmp(request -> uri, "/login") == 0 || strcmp(request -> uri, "/verifica") == 0)
        dynamicHandler(request, sockfd);
    else
        staticHandler(request, sockfd);
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
            struct HTTPRequest *requestStruct;
            requestStruct = parseHTTPRequest(request);
            requestHandler(requestStruct, newsockfd);
        }
        printf("ended\n");
    }

    return 0;
}

int main(int argc, char** argv){
    server();
    return 0;
}
