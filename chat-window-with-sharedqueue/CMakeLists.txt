# server/CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
project(self-chat)

set(CMAKE_CXX_STANDARD 17)

find_package(Curses REQUIRED system)

include_directories(${Curses_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/chat-window)

add_executable(${PROJECT_NAME} chat_main.cc chat_window.cc)
target_link_libraries(${PROJECT_NAME} ncurses pthread)
