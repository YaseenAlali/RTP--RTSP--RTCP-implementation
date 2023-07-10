#!/bin/sh
#this script is used to compile the server and the client
echo "compiling the server"
g++ server_main.cpp -o server -lX11 -L/usr/X11R6/lib -lpthread -lm

echo "compiled the server\ncompiling the client"
g++ client_main.cpp -o client -lX11 -L/usr/X11R6/lib -lpthread -lm

echo "compiled the client"

echo "compile.sh script finished\n"