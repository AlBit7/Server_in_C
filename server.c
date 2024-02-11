/**
 * SERVER PROGRAM 
 * 
 * must waiting to establish connection to clients at port 9999 of localhost.
 * When a client opens a connection main creates a child to handle the archive
 * being sent to the client.
 * 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "serveHTML.h"

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
