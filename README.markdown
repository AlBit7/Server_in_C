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