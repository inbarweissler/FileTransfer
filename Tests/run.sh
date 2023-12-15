#!/bin/bash

# Compile server
gcc ../server.c -o server

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "Server compiled successfully"
    
    # Run server in the background
    ./server &
    
    # Compile client
    gcc ../client.c -o client

    # Check if compilation was successful
    if [ $? -eq 0 ]; then
        echo "Client compiled successfully"
        
        # Run client
        ./client

    else
        echo "Error: Client compilation failed"
    fi

else
    echo "Error: Server compilation failed"
fi
