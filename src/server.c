/*
 * File: server.c
 * Author: Inbar R. Weissler
 * Created on: December 15, 2023
 * Description: Server-side code for sending files to connected clients.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdbool.h>
#include <pthread.h>

/******************
 * Global Constants
 ******************/
#define PORT_NUM 1908
#define OUT_BUFFER_SIZE 1024 //outbound buffer size
#define IN_FILE_NAME_BUFFER_SIZE 128 //inbound buffer size
#define FILE_DIR "./server_files/"
#define MAX_CLIENTS 5

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
void *send_file(void *socket_desc);


int main() {
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
    if (listen(server_socket, MAX_CLIENTS) == -1) { // accept upto max incoming connections upon success
        perror("Error: listening failure");
        close(server_socket); // close the server socket before exiting
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d\n", PORT_NUM);

    while(1){
        // Accepting a new connection
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
        // if succeed, return a new socket descriptor for the client communication

        if (client_socket == -1) {
            perror("Error: cannot accepting connection");
            continue;
        }

        // Print connection information
        printf("Client is connected\n");

        // Handle the connection in a new thread
        pthread_t thread;
        int *client_socket_ptr = malloc(sizeof(int));
        *client_socket_ptr = client_socket;

        // Sending file to the client
        if (pthread_create(&thread, NULL, send_file, (void *)client_socket_ptr) < 0) {
            perror("Error creating thread");
            free(client_socket_ptr);
            close(client_socket);
        }

        // Detach the thread to avoid memory leaks
        pthread_detach(thread);

    }

    close(server_socket); // close the server socket before exiting

    printf("exiting\n");
    
    return 0;
}


void *send_file(void *socket_desc) {
    int client_socket = *(int *)socket_desc;

    char file_name_buffer[IN_FILE_NAME_BUFFER_SIZE];
    memset(file_name_buffer, 0, sizeof(file_name_buffer));

    // Receive the file name from the client
    if (recv(client_socket, file_name_buffer, sizeof(file_name_buffer), 0) < 0) {
        perror("Error: cannot receive file name");
        close(client_socket);
        return NULL;
    }

    // Make the file path
    char file_path[256];
    snprintf(file_path, sizeof(file_path), "%s%s", FILE_DIR, file_name_buffer);

    // Read a file and returns a pointer
    FILE *file = fopen(file_path, "rb");
    if (file == NULL) {
        perror("Error: cannot open file");
        return NULL;
    }

    // A temporary array to store the data before sending it to the client
    char msg[OUT_BUFFER_SIZE];

    // A param defining how many bytes were read from the file
    size_t bytesCnt;

    // Reads data into msg array, and send it to client over the specified socket
    while ((bytesCnt = fread(msg, 1, sizeof(msg), file)) > 0) {
        send(client_socket, msg, bytesCnt, 0);
    }

    // Closing the file and client socket
    fclose(file);
    close(client_socket);
    free(socket_desc);
    return NULL;
}