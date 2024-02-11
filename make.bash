#!/bin/bash

# Nome del file eseguibile da generare
EXECUTABLE="server"

# Compilazione dei file sorgente
gcc -c -o serveHTML.o serveHTML.c
gcc -c -o server.o server.c

# Linking dei file oggetto per creare l'eseguibile finale
gcc -o $EXECUTABLE server.o serveHTML.o

# Pulizia dei file oggetto intermedi
rm *.o

echo "Compilazione completata. Eseguibile $EXECUTABLE generato."