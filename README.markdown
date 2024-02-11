# HTTP server in C

> mission is to create a well designed library to build web applications with

Now it's still early on fase

come fare: 
- creare una libreria con gli headers + macro
- creare il make file per compilare tutto
- pensare ad un modo per interfacciarsi con tutto

Creare una socket:

socklen_t initAddress(uint16_t port);
socklen_t lenSocket = initAddress(PORT);

int initSocket();
int socket = initSocket();

Sarebbe utile creare una struttura dati per gestire address e socket contemporaneamente