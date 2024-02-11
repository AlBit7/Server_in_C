#include "serveHTML.h"

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

void serviHTML(char *path, int socket) {

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
        exit(FAIL);
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
