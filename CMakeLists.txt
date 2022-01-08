cmake_minimum_required(VERSION 3.21)
project(rc_project)

set(CMAKE_C_STANDARD 99)

add_executable(client client/main.c client/centralized_messaging/commands.h client/interface.h client/centralized_messaging/commands.c client/parsing.h)
add_executable(server server/main.c server/parsing.h server/state/operations.c server/state/operations.h server/command_args_parsing.h server/connection.h server/command_handling.h)