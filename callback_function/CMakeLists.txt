cmake_minimum_required(VERSION 3.13)
project(callbacktest)      

find_package (Threads)

# Add main.cpp file of project root directory as source file
set(SOURCE_FILES 
    src/main.c 
    src/callback_thread.c)

# Add executable target with source files listed in SOURCE_FILES variable
add_executable(callbacktest ${SOURCE_FILES})

TARGET_LINK_LIBRARIES( callbacktest 
    pthread)