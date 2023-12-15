/*
 * File: client.c
 * Author: Inbar R. Weissler
 * Created on: December 15, 2023
 * Description:
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>


/******************
 * Global Constants
 ******************/
#define SERVER_IP "127.0.0.1" // assuming that the server runs on the same computer
#define PORT_NUM 1908
#define BUFFER_SIZE 1024


/****************
 * Function Declarations
 ****************/

/*
 * Receives the content of a file from a connected server over a specified socket.
 * Parameters:
 *   - server_socket (int): The socket descriptor of the connected server.
 *   - file_name (const char *): The name or path of the file to be received.
 * Returns:
 *   - None
 */
void receive_file(int server_socket, const char *file_name) {
    // open the file in write mode
    FILE *file = fopen(file_name, "wb");

    if (file == NULL) {
        perror("Error: cannot open file");
        return;
    }

    char msg[BUFFER_SIZE];
    ssize_t bytesCnt;

    // receive data from the server over the specied server_socket
    while ((bytesCnt = recv(server_socket, msg, sizeof(msg), 0)) > 0) {
        fwrite(msg, 1, bytesCnt, file); // write msg to the file

        // Print received data to the screen
        // TODO: delete this part later, this is for debug
        fwrite(msg, 1, bytesCnt, stdout);
    }

    // closing the file
    fclose(file);
}


int main() {
    // Creating a socket
    int client_socket; // store the socket descriptor for the client
    

    // assuming IPv4, and using TCP protocol
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Error: cannot create socket");
        exit(EXIT_FAILURE);
    }

    // Set up server address struct
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr)); // clearing server_addr struct
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP); // converts the server's IP to binary representation
    server_addr.sin_port = htons(PORT_NUM); // convers the port number to network byte order


    // Connecting to the server
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error: cannot connect to the server");
        close(client_socket); // close the client socket before exiting
        exit(EXIT_FAILURE);
    }

    printf("Connected to server at %s\n", SERVER_IP);

    // Receive the file from the server, and naming it received_file
    // TODO: generate name based on the file that was received
    receive_file(client_socket, "received_file.txt");

    close(client_socket); // close the client socket before exiting

    return 0;
}