- command_handling.h:
    - separar em dois ficheiros handle_tcp_command.h e handle_udp_command.h
    - no handle_tcp_command.h meter o codigo de cada uma das funcoes numa funcao separada

- command_args_parsing.h:
    - criar um command_args_parsing.c com um main() onde se faz o compile dos regex's em vez de acontecer sempre que se chama as funcoes de Parse