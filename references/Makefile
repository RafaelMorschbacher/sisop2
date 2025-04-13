# Defining variables
CXX = g++
CXXFLAGS = -Wall -pthread
CLIENT = src/client/client_handler
SERVER = src/server/server_handler
CLIENT_SRC = src/client/client_handler.cpp
SERVER_SRC = src/server/server_handler.cpp
LOG = src/log_log.log
PORT ?= 4000


# # Default target
# all: $(CLIENT) $(SERVER) $(LOG)

# # Rule to compile the client
# $(CLIENT): $(CLIENT_SRC) | src/client
# 	$(CXX) $(CXXFLAGS) -o $(CLIENT) $(CLIENT_SRC)

# # Rule to compile the server
# $(SERVER): $(SERVER_SRC) | src/server
# 	$(CXX) $(CXXFLAGS) -o $(SERVER) $(SERVER_SRC)

# # Rule to create the log file
# $(LOG): | src/
# 	touch $(LOG)

# # Rule to clean generated files
# clean:
# 	rm -f $(CLIENT) $(SERVER)
# 	rm -f $(LOG)

# # Rule to run the client
# run-client: $(CLIENT)
# 	./$(CLIENT) $(PORT)

# # Rule to run the client with input
# run-client-f: $(CLIENT)
# 	./$(CLIENT) $(PORT) < $(FILE)

# # Rule to run the server
# run-server: $(SERVER)
# 	./$(SERVER) $(PORT)

# .PHONY: all clean run-client run-server
# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -pthread
INCLUDES = -Isrc/structs -Isrc/utils -Isrc/services -Isrc/server -Isrc/client -Isrc/global_attributes

# Source files
CLIENT_SRC = src/client/client_handler.cpp
SERVER_SRC = src/server/server_handler.cpp

# Output binaries
CLIENT_TARGET = client_app
SERVER_TARGET = server_app

# Object files for server and client
CLIENT_OBJS = src/client/client_handler.o src/utils/Utils.o src/global_attributes/Global.o src/services/InterfaceService.o src/services/DiscoveryService.o src/services/ProcessingService.o
SERVER_OBJS = src/server/server_handler.o src/utils/Utils.o src/global_attributes/Global.o src/services/InterfaceService.o src/services/DiscoveryService.o src/services/ProcessingService.o

# Default target: build both client and server
all: $(CLIENT_TARGET) $(SERVER_TARGET)

# Link the server binary
$(SERVER_TARGET): $(SERVER_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lpthread

# Link the client binary
$(CLIENT_TARGET): $(CLIENT_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lpthread

# Compile the server object files
src/server/server_handler.o: src/server/server_handler.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Compile the client object files
src/client/client_handler.o: src/client/client_handler.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Compile each of the other object files
src/utils/Utils.o: src/utils/Utils.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

src/global_attributes/Global.o: src/global_attributes/Global.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

src/services/InterfaceService.o: src/services/InterfaceService.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

src/services/DiscoveryService.o: src/services/DiscoveryService.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

src/services/ProcessingService.o: src/services/ProcessingService.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(CLIENT_OBJS) $(SERVER_OBJS) $(CLIENT_TARGET) $(SERVER_TARGET)

.PHONY: all clean
