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

idealmente vorrei delle funzioni (come falsk) che gestiscono i vari tipi di richieste. 
Dinamica: 
- arriva la richiesta
- la parso con "abc = receveFromClient()" questo mi ritorna tutti i dati parsati
- capire cosa fare con quella richiesta -> "routeURI(abc)" posso quindi chiamare una funzione che è fatta per gestire quel tipo di richiesta
- la funzione "handler..." risponde alla richiesta con una pagina specifica o delle operazioni che servono all'utente

In più fare un funzione send() che manda al browser dei dati