#include "serveHTML.h"

Socket_t initSocket() {

    Socket_t soc = socket(AF_INET, SOCK_STREAM, 0);
    if (soc == -1) handleError("socket");

    return soc;

}

Address_t initAddress(bool isServer) {

    Address_t address;

    if (isServer) {
        address.addr.sin_family = AF_INET;         // server uses IPv4
        address.addr.sin_addr.s_addr = INADDR_ANY; // accept any incoming connections
        address.addr.sin_port = htons(PORT);       // on the port PORT
    }

    address.len = sizeof(address.addr);

    return address;

}

void bindStA(Socket_t soc, Address_t *addr) {
    if (
        bind(
            (int)soc, 
            (struct sockaddr *)&addr->addr, 
            addr->len
        )
    ) handleError("bind");
}

void listenS(Socket_t soc) {
    if (listen(soc, MAX_CONNECTIONS)) handleError("listen");
}

Socket_t acceptSgA(Socket_t soc, Address_t *address) {

    Socket_t clientSocket = (Socket_t)accept((int)soc, (struct sockaddr *)&address->addr, &address->len);
    if (clientSocket == -1) handleError("accept");

    return clientSocket;

}

Request_t receveFromClient(Socket_t soc) {

    // init
    Request_t request;
    request.methodSetted = false;
    request.uriSetted = false;

    // get request string
    char requestBuffer[BUFF];
    ssize_t bytesRead = recv(soc, requestBuffer, sizeof(requestBuffer), 0);
    if (bytesRead == -1) handleError("recv");
    if (bytesRead == 0) {
        close(soc); // the client sent an empty request
        puts("client sent empty reques");
    }
    requestBuffer[bytesRead] = '\0'; // Null-terminate the buffer

    // parse the client request and fill out request variable
    size_t i = 0;
    char buffer[BUFF] = { 0 };
    for (char *c = requestBuffer; *c; ++c) {

        // the idea is to iterate with another buffer to get 
        if (*c != ' ') buffer[i++] = *c; // add in buffer
        else { // when I find space
            buffer[i] = '\0'; // add terminator

            // if I still heve to fill method in
            if (!request.methodSetted) {
                if (!strcmp(buffer, "GET")) request.method = GET;
                else if (!strcmp(buffer, "POST")) request.method = POST;
                else request.method = NOT_SUPPORTED;
                request.methodSetted = true;
            } else if (!request.uriSetted) { // if I still heve to fill uri in
                strncpy(request.uri, buffer, i);
                request.uri[i+1] = '\0';
                request.uriSetted = true;
                return request;
            }
            // poi posso aggiungere altre informazioni 
            // che estraggo dalla richiesta ...

            i = 0; // reset buffer load at 0
        }

    }

}

void manageRequest(Socket_t clientSocket, Request_t request) {

    // manege the request and send data to specific function that handles it

    // is request GET, POST or ...?
    switch (request.method) {
    
        case GET:
            /* code */
            break;

        case POST:
            /* code */
            break;
    
        default:
            Responce_t responce = {
                .headers = "",
                .contenuto = ""
            };
            send(clientSocket, responce);
            break;
    }

}

// function that sends a responce type to a socket 
void send(Socket_t socket, Responce_t responce) {

    ssize_t bytes_written = 0;

    // write the header:
    bytes_written = write(socket, responce.headers, strlen(responce.headers));
    if (bytes_written == -1) handleError("write");

    // write contents:
    bytes_written = write(socket, responce.contenuto, strlen(responce.contenuto));
    if (bytes_written == -1) handleError("write");

}

char *routeURI(char *uri) {

    // ToDo implement a map to link requested with served

    if (!strcmp(uri, "/#") || !strcmp(uri, "/") || !strcmp(uri, "/index.html"))
        strcpy(uri, "/index.html");
    else if (!strcmp(uri, "/style.css") || !strcmp(uri, "/static/style.css"))
        strcpy(uri, "/static/style.css");
    else if (!strcmp(uri, "/favicon.ico"))
        strcpy(uri, "/static/favicon.ico");
    else if (!strcmp(uri, "/JS/main.js"))
        strcpy(uri, "/JS/main.js");
    else
        strcpy(uri, "/notFound.html");

    //printf("serving: %s\n", uri);
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

// DEPRECATED

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
