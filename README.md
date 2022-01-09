- server/command_handling.h:
    - separar em dois ficheiros handle_tcp_command.h e handle_udp_command.h
    - no handle_tcp_command.h meter o codigo de cada uma das funcoes numa funcao separada

- server/command_args_parsing.h:
    - criar um command_args_parsing.c com um main() onde se faz o compile dos regex's em vez de acontecer sempre que se chama as funcoes de Parse

- server:
    - criar funcoes para fazer print das msg e dos erros, para evitar os "if (verbose) {}" em todo o lado

- client/centralized_messaging/:
    - criar um interface.h

- estamos a aceitar o GID "00" em casos em que nao se devia aceitar, faz com que o erro que enviamos ao cliente fique diferente, por exemplo mandamos "NOK" em vez de mandarmos "E_GRP"

- usar int em vez de char* o mais cedo possivel nas funcoes do server