- server/command_handling.h:
    - separar em dois ficheiros handle_tcp_command.h e handle_udp_command.h

- server:
    - criar funcoes para fazer print das msg e dos erros, para evitar os "if (verbose) {}" em todo o lado

- client/centralized_messaging/:
    - criar um interface.h

- server/state/operations.c:235:22 warning do snprintf

- mudar client -> User e server -> DS