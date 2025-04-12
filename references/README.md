# Client-Server


## How to compile
To compile the project, run the `make` command at the root of the directory.
The default port is set to be 4000 if it's not specified

## How to run

To run the server:
```sh
make run-server
# or
make run-server PORT=<PORT>
```

To run client with manual user input:
```sh
make run-client 
# or
make run-client PORT=<PORT>
```

To run client with input file:
```sh
make run-client-f PORT=<PORT> FILE=<input_file.txt>
```

Make sure to run:
```sh
make clean
```
before re-comipiling