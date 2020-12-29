
project(SocketLibrary)
cmake_minimum_required(VERSION 3.13)
set(CMAKE_CXX_STANDARD 11)

add_compile_options( -Wall -pthread -std=c++11)

set(THREADS_PREFER_PTHREAD_FLAG ON)

add_library(SocketLibrary SHARED
        AskingThread.cpp
        ClientThreads.cpp
        ListeningThread.cpp
        Particepant.cpp
        RequestHandlers.cpp
        RequestObject.cpp
        RequestObjects.cpp
        RequestObjectDispatcher.cpp
        SocketServer.cpp
        SocketClient.cpp
        SendListenSockets.cpp
        RequestParser.cpp RequestParser.h)
target_link_libraries(SocketLibrary pthread)