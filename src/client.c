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
#define OUT_BUFFER_SIZE 1024 //outbound
#define FILE_DIR "./client_files/"

/****************
 * Function Declarations
 ****************/
//char* get_file_name2(int file_id, int version_num);

/*
 * Receives the content of a file from a connected server over a specified socket.
 * Parameters:
 *   - server_socket (int): The socket descriptor of the connected server.
 *   - file_name (const char *): The name or path of the file to be received.
 * Returns:
 *   - None
 */
void receive_file(int server_socket, const char *file_name);


int main(int argc, char *argv[]) {
    // Check if the correct number of arguments are provided
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file_name>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *file_name = argv[1];

    // TODO: validate file_name length

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

    // Sending file_name to the server    
        if (send(client_socket, file_name, strlen(file_name), 0) == -1) {
        perror("Error: cannot sending file name");
        close(client_socket);
        exit(EXIT_FAILURE);
    }


    receive_file(client_socket, file_name);

    close(client_socket); // close the client socket before exiting

    return 0;
}


void receive_file(int server_socket, const char *file_name) {
    // make the file path
    char file_path[256];
    snprintf(file_path, sizeof(file_path), "%s%s", FILE_DIR, file_name);
    // open the file in write mode
    FILE *file = fopen(file_path, "wb");

    if (file == NULL) {
        perror("Error: cannot open file");
        return;
    }
    
    char msg[OUT_BUFFER_SIZE];
    ssize_t bytesCnt;

    // receive data from the server over the specied server_socket
    while ((bytesCnt = recv(server_socket, msg, sizeof(msg), 0)) > 0) {
        fwrite(msg, 1, bytesCnt, file); // write msg to the file

        // Print received data to the screen
        // TODO: delete this part later, this is for debug
        fwrite(msg, 1, bytesCnt, stdout);
    }

    // closing the file and server socket
    fclose(file);
    close(server_socket);
}