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

    // init phase
    Address_t serverAddress = initAddress(true);
    Address_t clientAddress = initAddress(false);
    Socket_t  serverSocket  = initSocket();

    // binding serverSocket to the address
    bindStA(serverSocket, &serverAddress);

    // listen the socket file descriptor for incoming connections
    // if more then MAX_CONNECTIONS arrive the server will refuse to connect
    listenS(serverSocket);
    puts("listening ...");

    // --------------- ACCEPTING CONNECTIONS ---------------

    while (1) {

        // accepting a connection socket and storing its address in clientAddress:
        Socket_t clientSocket = acceptSgA(serverSocket, &clientAddress);

        // creating a child process to handle the client connection
        pid_t pid = fork();
        if (pid == -1) handleError("fork");
        else if (pid == 0) { // child process
            close(serverSocket);  // child doesn't need the listener

            // reading the request URI
            Request_t requestFromClient = receveFromClient(clientSocket);

            printf("%d:%d: <%s>\n", clientAddress.addr.sin_addr.s_addr, clientAddress.addr.sin_port, requestFromClient.uri);

            // what page should I serve?
            // and serve it!
            serviHTML(routeURI(requestFromClient.uri), clientSocket);

            close(clientSocket);  // close client socket at the end
            exit(SUCCESS);        // child dies

        } else close(clientSocket);  // parent doesn't need the client socket

    }

    // close the socket
    if (!close(serverSocket)) handleError("close socket");

    return SUCCESS;

}
