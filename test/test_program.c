#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <sys/stat.h>
#include <pthread.h> 

// Compile client and server
void compile_client_server() {
    system("gcc -pthread src/server.c -o bin/server"); // Compile the server with pthread library
    system("gcc src/client.c -o bin/client");
}

// Run client and server
void run_client_server() {
    compile_client_server();
    printf("%s", __FILE__);
    
    system("rm -rf ./client_files");
	system("mkdir -p ./client_files");

    system("rm -rf ./log");
	system("mkdir -p ./log");

    system("bin/server &");
    system("bin/client 1 11_1.txt &");
}

// Check if output file exists
int check_file_exists() {
    return access("./client_files/11_1.txt", F_OK);
}

int main() {
    compile_client_server();
    run_client_server();
    sleep(5); // Sleep for 1 second
    assert(check_file_exists() == 0);
    printf("test passed\n");

    return 0;
}
