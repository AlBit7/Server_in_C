#ifndef serveHTML
#define serveHTML

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdbool.h>

#define PORT 9999
#define MAX_CONNECTIONS 10

#define PATH_TO_PAGE "/home/albi/Desktop/albi/programmi/Server_in_C"
#define SIZE_MAX 100
#define MAX_MESSAGE_SIZE 100
#define IP_GOOGLE "142.250.200.100"
#define HTML_PORT 80 // 443 con https
#define BUFF 1024

// errors
#define FAIL -1
#define SUCCESS 0
#define handleError(msg) \
    do { perror(msg); exit(FAIL); } while (0)

// methods
#define GET 1
#define POST 2
#define NOT_SUPPORTED 0

// data structures
typedef int Socket_t; // to represent socket

typedef struct { // to represent address
    struct sockaddr_in addr;
    socklen_t len;
} Address_t; 

typedef struct { // to represent client's request
    char uri[BUFF];
    unsigned short method;
    bool methodSetted;
    bool uriSetted;
} Request_t;

typedef struct {
    char *headers;
    char *contenuto;
} Responce_t;


Socket_t  initSocket();
Address_t initAddress(bool);
void      bindStA(Socket_t, Address_t*);
void      listenS(Socket_t);
Socket_t  acceptSgA(Socket_t, Address_t*);
Request_t receveFromClient(Socket_t);
void      manageRequest(Socket_t, Request_t);
void      send(Socket_t, Responce_t);

void child(int);
void serviHTML(char*, int);
char *parseURI(const char*);
char *routeURI(char*);
char *concatenaStringhe(const char*, const char*);

#endif