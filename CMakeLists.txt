cmake_minimum_required(VERSION 3.10)

# Nome do projeto
project(Sisop2)

# Configurar a versão do C++
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")

include_directories(
    socket/classes/include
    socket/include
    client/classes/include
    server/classes/include
)

add_executable(server_executable
    server/main_server.cpp
    socket/classes/UdpServer.cpp
)

add_executable(client_executable
    client/main_client.cpp
    socket/classes/UdpClient.cpp
)