/*
 * File: server.c
 * Author: Inbar R. Weissler
 * Created on: December 15, 2023
 * Description:
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdbool.h>

/******************
 * Global Constants
 ******************/
#define PORT_NUM 1908
#define OUT_BUFFER_SIZE 1024 //outbound
#define IN_FILE_NAME_BUFFER_SIZE 128 //inbound
#define FILE_DIR "./server_files/"

/****************
 * Function Declarations
 ****************/
/*
 * Sends the content of a file to a connected client over a specified socket.
 * Parameters:
 *   - client_socket (int): The socket descriptor of the connected client.
 *   - file_name (const char *): The name of the file to be sent.
 * Returns:
 *   - None
 */
void send_file(int client_socket);


int main() {
    // TODO: enable upto MAX_CLIENTS connections in parallel

    // 1) Create socket
    int server_socket, client_socket;

    // assuming IPv4, and using TCP protocol
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Error: cannot create socket");
        exit(EXIT_FAILURE);
    }

    // 2) Server address setup
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    // Clearing server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; // binds the server to all available network interfaces
    server_addr.sin_port = htons(PORT_NUM); // convers the port number to network byte order

    // Socket binding
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error: binding failure");
        close(server_socket); // close the socket before exiting
        exit(EXIT_FAILURE);
    }

    // Listening for connections
    if (listen(server_socket, 1) == -1) { // accept incoming connection upon success
        perror("Error: listening failure");
        close(server_socket); // close the server socket before exiting
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d\n", PORT_NUM);

    // Accepting a new connection
    client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
    // if succeed, return a new socket descriptor for the client communication

    if (client_socket == -1) {
        perror("Error: cannot accepting connection");
        close(server_socket); // close the server socket before exiting
        exit(EXIT_FAILURE);
    }

    // Print connection information
    printf("Client is connected\n");

    // Sending file to the client
    send_file(client_socket);

    close(client_socket); // close the client socket before exiting
    close(server_socket); // close the server socket before exiting

    printf("exiting\n");
    
    return 0;
}


void send_file(int client_socket) {
    char file_name_buffer[IN_FILE_NAME_BUFFER_SIZE];
    memset(file_name_buffer, 0, sizeof(file_name_buffer));

    // Receive the file name from the client
    if (recv(client_socket, file_name_buffer, sizeof(file_name_buffer), 0) < 0) {
        perror("Error: cannot receive file name");
        close(client_socket);
        return;
    }

    // make the file path
    char file_path[256];
    snprintf(file_path, sizeof(file_path), "%s%s", FILE_DIR, file_name_buffer);

    // read a file and returns a pointer
    FILE *file = fopen(file_path, "rb");
    if (file == NULL) {
        perror("Error: cannot open file");
        return;
    }

    // a temporary array to store the data before sending it to the client
    char msg[OUT_BUFFER_SIZE];

    // a param defining how many bytes were read from the file
    size_t bytesCnt;

    // reads data into msg array, and send it to client over the specified socket
    while ((bytesCnt = fread(msg, 1, sizeof(msg), file)) > 0) {
        send(client_socket, msg, bytesCnt, 0);
    }

    // closing the file
    fclose(file);
}