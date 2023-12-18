CC = gcc
CFLAGS = -Wall -Wextra
SERVER_SRC = ./src/server.c
CLIENT_SRC = ./src/client.c


test: clean all run_server run_client

all: files server client

server: $(SERVER_SRC)
	$(CC) $(CFLAGS) $^ -o ./bin/$@ -lpthread
	@echo "Server compiled successfully"

client: $(CLIENT_SRC)
	$(CC) $(CFLAGS) $^ -o ./bin/$@
	@echo "Client compiled successfully"

files:
	mkdir -p bin
	mkdir -p client_files
	mkdir -p log

run_server: server
	./bin/server &

run_client: client
	./bin/client 1 11_1.txt &
	./bin/client 2 11_2.txt &
	./bin/client 3 12_1.txt &
	./bin/client 4 12_2.txt &
	./bin/client 5 13_1.txt &
	./bin/client 6 13_2.txt &

clean:
	rm -f ./bin/server ./bin/client
	rm -rf ./client_files
	rm -rf ./log
