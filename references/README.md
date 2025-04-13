# Client-Server


## How to compile
```sh
make server_app
make client_app

## How to run

To run the server:
```sh
./server_app <port>

To run client with manual user input:
```sh
./client_app <port>


To run client with input file:
```sh
make run-client-f PORT=<PORT> FILE=<input_file.txt>
```

Make sure to run:
```sh
make clean
```
before re-comipiling