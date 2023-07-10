#!/bin/sh

#this script is used to remove the server and client binaries

echo "removing the server"
rm -f server
echo "removed the server\nremoving the client"
rm -f client
echo "removed the client"

echo "clean.sh script finished\n"