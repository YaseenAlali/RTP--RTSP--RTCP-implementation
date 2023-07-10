#!/bin/sh
#this script calls clean.sh to clean old binaries and compile.sh to compile the server and the client
#it then opens to new terminal windows that run the server and the client 

./clean.sh
./compile.sh

gnome-terminal --title="server" --command="bash -c './server; $SHELL'"
sleep 3
gnome-terminal --title="client" --command="bash -c './client; $SHELL'"