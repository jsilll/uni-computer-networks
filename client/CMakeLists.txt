cmake_minimum_required(VERSION 3.21)
project(client C)

set(CMAKE_C_STANDARD 99)

add_executable(client main.c centralized_messaging_api.h centralized_messaging_parsing.h client_interface.h)
