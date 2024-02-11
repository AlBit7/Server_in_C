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

#define PORT 9999
#define MAX_CONNECTIONS 10

#define PATH_TO_HTML "/home/albi/Desktop/Universita/OS - sistemas operativos/practica/practica 2/es1/pagina/index.html"
#define PATH_TO_PAGE "/home/albi/Desktop/Universita/OS - sistemas operativos/practica/practica 2/es1/pagina"
#define SIZE_MAX 100
#define MAX_MESSAGE_SIZE 100
#define IP_GOOGLE "142.250.200.100"
#define HTML_PORT 80 // 443 con https

// errors
#define FAIL -1
#define SUCCESS 0
#define handleError(msg) \
    do { perror(msg); exit(FAIL); } while (0)

void child(int);
void serviHTML(char*, int);
char *parseURI(const char*);
char *routeURI(char*);
char *concatenaStringhe(const char*, const char*);

#endif