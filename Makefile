CC = gcc
CFLAGS = -Wall -Wextra
SERVER_SRC = ./src/server.c
CLIENT_SRC = ./src/client.c


test: clean all run_server run_client

all: server client files

server: $(SERVER_SRC)
	$(CC) $(CFLAGS) $^ -o ./bin/$@ -lpthread
	@echo "Server compiled successfully"

client: $(CLIENT_SRC)
	$(CC) $(CFLAGS) $^ -o ./bin/$@
	@echo "Client compiled successfully"

files:
	mkdir -p client_files

run_server: server
	mkdir -p bin
	./bin/server &

run_client: client
	./bin/client 11_1.txt
	./bin/client 11_2.txt
	./bin/client 12_1.txt
	./bin/client 12_2.txt
	./bin/client 13_1.txt
	./bin/client 13_2.txt

clean:
	rm -f ./bin/server ./bin/client
	rm -rf ./client_files
