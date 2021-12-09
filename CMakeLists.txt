cmake_minimum_required(VERSION 3.21)
project(rc_project C)

set(CMAKE_C_STANDARD 99)

add_executable(client client/main.c client/centralized_messaging/commands.h client/centralized_messaging/parsing.h client/interface.h client/centralized_messaging/commands.c)
add_executable(server server/main.c)