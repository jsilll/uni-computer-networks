cmake_minimum_required(VERSION 3.21)
project(rc_project)

set(CMAKE_C_STANDARD 99)

add_executable(client client/src/main.c client/src/command_args_parsing.h client/src/commands.h client/src/interface.h client/src/commands.c client/src/parsing.h)
add_executable(server server/src/main.c server/src/parsing.h server/src/operations.c server/src/operations.h server/src/command_args_parsing.h server/src/connection.h server/src/command_handling.h)