# Defining variables
CXX = g++
CXXFLAGS = -Wall -pthread
CLIENT = src/client/client_handler
SERVER = src/server/server_handler
CLIENT_SRC = src/client/client_handler.cpp
SERVER_SRC = src/server/server_handler.cpp
LOG = src/log_log.log
PORT ?= 4000


# Default target
all: $(CLIENT) $(SERVER) $(LOG)

# Rule to compile the client
$(CLIENT): $(CLIENT_SRC) | src/client
	$(CXX) $(CXXFLAGS) -o $(CLIENT) $(CLIENT_SRC)

# Rule to compile the server
$(SERVER): $(SERVER_SRC) | src/server
	$(CXX) $(CXXFLAGS) -o $(SERVER) $(SERVER_SRC)

# Rule to create the log file
$(LOG): | src/
	touch $(LOG)

# Rule to clean generated files
clean:
	rm -f $(CLIENT) $(SERVER)
	rm -f $(LOG)

# Rule to run the client
run-client: $(CLIENT)
	./$(CLIENT) $(PORT)

# Rule to run the client with input
run-client-f: $(CLIENT)
	./$(CLIENT) $(PORT) < $(FILE)

# Rule to run the server
run-server: $(SERVER)
	./$(SERVER) $(PORT)

.PHONY: all clean run-client run-server
