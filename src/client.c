/*
 * File: client.c
 * Author: Inbar R. Weissler
 * Created on: December 15, 2023
 * Description: Client-side code for receiving files from a server.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>


/******************
 * Global Constants
 ******************/
#define SERVER_IP "127.0.0.1" // assuming that the server runs on the same computer
#define PORT_NUM 1908
#define OUT_BUFFER_SIZE 1024 // outbound buffer size
#define FILE_DIR "./client_files/"
#define LOG_FILE_DIR "./log/client_log.txt"

/****************
 * Function Declarations
 ****************/

FILE *log_file; // Global variable to store the log file pointer

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
    log_file = fopen(LOG_FILE_DIR, "a"); // Open the log file in append mode
    // TODO: consider changing to write mode

    if (log_file == NULL) {
        perror("Error: cannot open log file");
        exit(EXIT_FAILURE);
    }

    // Check if the correct number of arguments are provided
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <file_name>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int client_id = atoi(argv[1]);
    const char *file_name = argv[2]; // TODO: validate file_name length

    fprintf(log_file, "ID %d, file-name %s\n", client_id, file_name); 

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
    
    // Print current time
    time_t t;
    t = time(NULL);
    struct tm tm;
	tm = *localtime(&t);
    fprintf(log_file, "#%d Current Time: %d:%d:%03d\n", client_id, tm.tm_hour, tm.tm_min, tm.tm_sec);

    fprintf(log_file, "#%d Connected to server at %s\n", client_id, SERVER_IP);

    // Sending file_name to the server    
        if (send(client_socket, file_name, strlen(file_name), 0) == -1) {
        perror("Error: cannot sending file name");
        close(client_socket);
        exit(EXIT_FAILURE);
    }


    receive_file(client_socket, file_name);

    close(client_socket); // close the client socket before exiting
    fclose(log_file); // Close the log file before exiting
    return 0;
}


void receive_file(int server_socket, const char *file_name) {
    // Make the file path
    char file_path[256];
    snprintf(file_path, sizeof(file_path), "%s%s", FILE_DIR, file_name);
    
    // Open the file in write mode
    FILE *file = fopen(file_path, "wb");

    if (file == NULL) {
        perror("Error: cannot open file");
        return;
    }
    
    char msg[OUT_BUFFER_SIZE];
    ssize_t bytesCnt;

    // Receive data from the server over the specied server_socket
    while ((bytesCnt = recv(server_socket, msg, sizeof(msg), 0)) > 0) {
        fwrite(msg, 1, bytesCnt, file); // write msg to the file

        // Print received data to the screen
        // TODO: delete this part later, this is for debug
        fwrite(msg, 1, bytesCnt, stdout);
    }

    // Closing the file and server socket
    fclose(file);
    close(server_socket);
}