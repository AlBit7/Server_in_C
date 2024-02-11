/**
 * SERVER PROGRAM 
 * 
 * must waiting to establish connection to clients at port 9999 of localhost.
 * When a client opens a connection main creates a child to handle the archive
 * being sent to the client.
 * 
*/

#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

#define PORT 9999
#define MAX_CONNECTIONS 10

#define PATH_TO_HTML "/home/albi/Desktop/Universita/OS - sistemas operativos/practica/practica 2/es1/pagina/index.html"
#define PATH_TO_PAGE "/home/albi/Desktop/Universita/OS - sistemas operativos/practica/practica 2/es1/pagina"
#define SIZE_MAX 100
#define MAX_MESSAGE_SIZE 100
#define IP_GOOGLE "142.250.200.100"
#define HTML_PORT 80 // 443 con https

#define FAIL -1
#define SUCCESS 0
#define handleError(msg) \
    do { perror(msg); exit(FAIL); } while (0)

void child(int);
void serveHTML(char*, int);
char *parseURI(const char*);
char *routeURI(char*);
char *concatenaStringhe(const char*, const char*);

int main() {

    // create the connection reciving capability ...
    // To accept connections, the following steps are performed:
    //  (1)  A socket is created with socket(2).
    //  (2)  The  socket is bound to a local address using bind(2), so that other sockets may be connect(2)ed to it.
    //  (3)  A willingness to accept incoming connections and a queue limit for incoming connections are specified with listen().
    //  (4)  Connections are accepted with accept(2).

    // --------------- INIT ---------------

    // create the two addresses structs for server and to reference client
    struct sockaddr_in serverAddress, clientAddress;
    // init the lengths of the addresses (for bind and accept)
    socklen_t lenServerSocket = sizeof(serverAddress);
    socklen_t lenClientSocket = sizeof(clientAddress);
    // init the serverAddress
    serverAddress.sin_family = AF_INET;         // server uses IPv4
    serverAddress.sin_addr.s_addr = INADDR_ANY; // accept any incoming connections
    serverAddress.sin_port = htons(PORT);       // on the port PORT

    // --------------- CONNECTING THE SOCKET ---------------

    // creating socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) handleError("socket");

    // binding serverSocket to the address
    if (bind(serverSocket, (struct sockaddr *)&serverAddress, lenServerSocket)) handleError("bind");
    // now the address is binded to the socket

    // listen the socket file descriptor for incoming connections
    // if more then MAX_CONNECTIONS arrive the server will refuse to connect
    if (listen(serverSocket, MAX_CONNECTIONS)) handleError("listen");
    puts("listening ...");

    // --------------- ACCEPTING CONNECTIONS ---------------

    while (1) {

        // accepting a connection socket and storing its address in clientAddress:
        int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &lenClientSocket);
        if (clientSocket == -1) handleError("accept");

        // creating a child process to handle the client connection
        pid_t pid = fork();
        if (pid == -1) handleError("fork");
        else if (pid == 0) {

            // reading the request URI
            char requestBuffer[1024];
            ssize_t bytesRead = recv(clientSocket, requestBuffer, sizeof(requestBuffer), 0);
            if (bytesRead == -1) handleError("recv");
            if (bytesRead == 0) {
                close(clientSocket);
                continue;
            }
            requestBuffer[bytesRead] = '\0'; // Null-terminate the buffer

            // Parse the URI from the request
            char *uri = parseURI(requestBuffer);
            if (uri == NULL) {
                // Handle invalid request
                close(clientSocket);
                continue;
            }

            printf("%d:%d: %s\n", clientAddress.sin_addr.s_addr, clientAddress.sin_port, uri);
            // child process
            close(serverSocket);  // child doesn't need the listener

            // what page should I serve?
            // and serve it!
            serveHTML(routeURI(uri), clientSocket);

            close(clientSocket);  // close client socket at the end
            exit(SUCCESS);        // child dies

        } else close(clientSocket);  // parent doesn't need the client socket

    }

    // close the socket
    if (!close(serverSocket)) handleError("close socket");

    return SUCCESS;

}

void child(int clientSocket) {

    // connect to google as a client


    // Build HTTP request
    const char *request_template = "GET / HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n";
    char GET_request[SIZE_MAX];
    snprintf(GET_request, SIZE_MAX, request_template, IP_GOOGLE);

    // create the socket
    int childSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (childSocket == -1) handleError("socket");

    // specify the address to connect to
    struct sockaddr_in googleServerAddress;
    googleServerAddress.sin_family = AF_INET;
    googleServerAddress.sin_port = htons(HTML_PORT);
    if (inet_pton(AF_INET, IP_GOOGLE, &googleServerAddress.sin_addr) <= 0) 
        handleError("IP conversion error");

    // connect to the server
    if (connect(
        childSocket, 
        (struct sockaddr *)&googleServerAddress, 
        sizeof(googleServerAddress)
    ) < 0) handleError("connect");

    puts("connected to google...");

    // send the GET_request (previously built)
    if (send(childSocket, GET_request, strlen(GET_request), 0) == -1) 
        handleError("GET");

    // read from google's server connection 
    ssize_t bytes_read;
    char buffer[MAX_MESSAGE_SIZE] = {0};
    do {

        // fresh init of buffer
        memset(buffer, 0, sizeof(buffer));

        // read from the socket
        bytes_read = recv(childSocket, buffer, MAX_MESSAGE_SIZE-1, 0);
        if (bytes_read == -1) handleError("read");

        // send to the client if some bytes have been read
        if (bytes_read && write(clientSocket, buffer, bytes_read) == -1) 
            handleError("write");

    } while (bytes_read); 

}

char *routeURI(char *uri) {

    // ToDo implement a map to link requested with served

    if (!strcmp(uri, "/") || !strcmp(uri, "/index.html"))
        strcpy(uri, "/index.html");
    else if (!strcmp(uri, "/static/style.css"))
        strcpy(uri, "/static/style.css");
    else if (!strcmp(uri, "/favicon.ico"))
        strcpy(uri, "/static/favicon.ico");
    else if (!strcmp(uri, "/JS/main.js"))
        strcpy(uri, "/JS/main.js");
    else
        strcpy(uri, "/notFound.html");

    printf("serving: %s\n", uri);
    return concatenaStringhe(PATH_TO_PAGE, uri);

}

void serveHTML(char *path, int socket) {

    // read archive: 
    int file = open(path, O_RDONLY);
    if (file < 0) handleError("file opening: ");

    // sending the headers ... with keepalive ...
    char *header = "HTTP/1.1 200 OK\nConnection: keep-alive\n\n";
    write(socket, header, strlen(header));

    // buffer of SIZE_MAX bytes
    char tmp_bytes[SIZE_MAX] = { 0 };
    ssize_t bytes_read;
    ssize_t sent;

    // read the file SIZE_MAX bytes at a time and send it to the client
    do {

        // fresh init of buffer
        for (size_t i = 0; i < SIZE_MAX; ++i) tmp_bytes[i] = 0;
        
        // read and check
        bytes_read = read(file, tmp_bytes, SIZE_MAX);
        if (bytes_read == -1) handleError("read");

        sent += bytes_read;
        
        // send to the client if some bytes are read
        if (bytes_read) {
            ssize_t bytes_written = write(socket, tmp_bytes, bytes_read);
            if (bytes_written == -1) handleError("write");
        }

    } while (bytes_read);

    // printf("message sent: %ld bytes\n", sent);

}

char *parseURI(const char *request) {
    
    const char *start = strstr(request, "GET ");
    if (start == NULL) return NULL; // Invalid request
    
    start += 4; // Move to the start of the URI
    const char *end = strchr(start, ' ');
    if (end == NULL) return NULL; // Invalid request
    
    int length = end - start;
    char *uri = (char *)malloc(length + 1);
    if (uri == NULL) return NULL; // Memory allocation failed
    
    strncpy(uri, start, length);
    uri[length] = '\0'; // Null-terminate the string
    
    return uri;

}

char *concatenaStringhe(const char *str1, const char *str2) {
    // Calcola la lunghezza totale della stringa risultante
    int len1 = 0;
    while (str1[len1] != '\0') {
        len1++;
    }

    int len2 = 0;
    while (str2[len2] != '\0') {
        len2++;
    }

    int len_totale = len1 + len2;

    // Alloca memoria per la stringa risultante
    char *concatenata = (char *)malloc((len_totale + 1) * sizeof(char)); // +1 per il terminatore di stringa '\0'
    if (concatenata == NULL) {
        // Gestione dell'errore di allocazione di memoria
        perror("Errore di allocazione di memoria");
        exit(EXIT_FAILURE);
    }

    // Copia la prima stringa nella stringa risultante
    for (int i = 0; i < len1; i++) {
        concatenata[i] = str1[i];
    }

    // Concatena la seconda stringa alla fine della prima stringa
    for (int i = 0; i < len2; i++) {
        concatenata[len1 + i] = str2[i];
    }

    concatenata[len_totale] = '\0'; // Aggiungi il terminatore di stringa

    return concatenata;
}
