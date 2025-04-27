CXX = g++
CXXFLAGS = -Wall -pthread
CLIENT = src/client/client_handler
SERVER = src/server/server_handler
CLIENT_SRC = src/client/client_handler.cpp
SERVER_SRC = src/server/server_handler.cpp
LOG = src/log_log.log
PORT ?= 4000
FILE ?=

CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -pthread
INCLUDES = -Isrc/structs -Isrc/utils -Isrc/services -Isrc/server -Isrc/client -Isrc/global_attributes

CLIENT_SRC = src/client/client_handler.cpp
SERVER_SRC = src/server/server_handler.cpp

CLIENT_TARGET = client_app
SERVER_TARGET = server_app

CLIENT_OBJS = src/client/client_handler.o src/utils/Utils.o src/global_attributes/Global.o src/services/InterfaceService.o src/services/DiscoveryService.o src/services/ProcessingService.o
SERVER_OBJS = src/server/server_handler.o src/utils/Utils.o src/global_attributes/Global.o src/services/InterfaceService.o src/services/DiscoveryService.o src/services/ProcessingService.o

all: $(CLIENT_TARGET) $(SERVER_TARGET)

$(SERVER_TARGET): $(SERVER_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lpthread

$(CLIENT_TARGET): $(CLIENT_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lpthread

src/server/server_handler.o: src/server/server_handler.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

src/client/client_handler.o: src/client/client_handler.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

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

run-client: $(CLIENT_TARGET)
	@if [ -z "$(strip $(FILE))" ]; then \
		./$(CLIENT_TARGET) $(PORT); \
	else \
		./$(CLIENT_TARGET) $(PORT) < $(FILE); \
	fi

run-server: $(SERVER_TARGET)
	./$(SERVER_TARGET) $(PORT)

clean:
	rm -f $(CLIENT_OBJS) $(SERVER_OBJS) $(CLIENT_TARGET) $(SERVER_TARGET)

.PHONY: all clean run-client run-server
