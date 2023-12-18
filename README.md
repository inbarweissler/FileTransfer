# C Program - File Transfer Application

This repository contains a simple file transfer application written in C. The program consists of a server and multiple clients, allowing clients to request and receive files from the server.

## Prerequisites

- **GCC Compiler**: The program is developed using the GCC compiler.
- **POSIX Threads (libpthread)**: The server utilizes POSIX threads for handling multiple client connections.

## Getting Started

1. **Clone the Repository:**

    ```bash
    git clone <repository_url>
    cd <repository_directory>
    ```

2. **Build the Program:**

    ```bash
    make all
    ```

    This command compiles the server and client programs, creating the necessary directories and executable files.

3. **Run a Demo:**

    ```bash
    make demo
    ```

    The server will build the program and run it on a test case to demonstrate it functionality.

4. **Clean Up:**

    ```bash
    make clean
    ```

    This command removes the compiled binaries and temporary directories.

5. **Run Test:**

    Run the test file using the following commands:

    ```bash
    rm -f ./bin/test_program
    gcc test/test_program.c -o bin/test_program
    ./bin/test_program
    ```

## Directory Structure

- **bin:** Executable files are stored here.
- **client_files:** Clients' requested file copies are transfered to here.
- **server_files:** Server's provided files are stored here.
- **log:** Log files, including server logs, are stored here.
- **src:** Source code directory.
- **test:** Test files are stored here.

## Program Structure

- **server.c:** Server-side code for handling file requests.
- **client.c:** Client-side code for requesting files from the server.

## Notes

- The logs can be found in the `log/client_log.txt` file.
- Each client instance requests a specific file and saves it in the `client_files/` directory.
- The client will recieve the file with a certain delay, showcasing the request handling concurrency.

## Contributors

- Inbar R. Weissler

Feel free to contribute or report issues by creating a pull request or raising an issue in the repository.
