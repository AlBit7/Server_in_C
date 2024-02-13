# HTTP server in C

> mission is to create a well designed library to build web applications with

Now it's still early on fase

### How to use?

super easy:

```
chmod +x make.bash
./make.bash
./server
```

And start serving!

### High level overview

Sarebbe utile creare una struttura dati per gestire address e socket contemporaneamente un qualcosa del tipo:

```
Socket s = initSocket();
Address serverAddr = initAddress();
Address serverAddr = initAddress();

bindStA(s, &serverAddr);

listenS(s);

while(1)
   Socket client = acceptSgA(s, &clientAddr);
   Request requestFromClient = receveFromClient(client);
```

### Routing

Ora devo preparare la parte del routing:
arriva una stringa e io devo dire che file intende e rispondere a seconda di cosa deve servire il server

## Gestione dei metodi GET e POST

devo a vere un modo per gestire GET e POST
GET basta mangargli 

creazione della funzione di gestione delle richieste:
```
Responce_t manageRequest(Request_t request) {

    Responce_t responce;

    // is request GET, POST or ...?
    switch (request.method) {
    
        case GET:
            responce.headers = "HTTP/1.1 200 OK bruh adesso ti mando tutto";
            responce.contenuto = fileToText(routeURI(request.uri)); // file serving service
            break;

        case POST:
            responce.headers = "HTTP/1.1 405 Bruh va che POST non so ancora gestirlo";
            responce.contenuto = request.body; // just repete body in responce
            break;
    
        default:
            responce.headers = "HTTP/1.1 405 Cabbo vuoi fare?";
            responce.contenuto = "deh va che hai mandato un metodo che non conosco";
            break;
    }

    return responce;

}
```
la risposta viene poi mandata alla funzione sendR() che invia la risposta alla socket che ha inviato la richiesta:
```
// reading the request URI
Request_t requestFromClient = receveFromClient(clientSocket);

// create responce from request
Responce_t responce = manageRequest(requestFromClient);

// send responce
sendR(clientSocket, responce);
```
Questo è più o meno lo schema generale per rispondere alle richieste 