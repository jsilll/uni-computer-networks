# Centralized Messaging Server and Client

## Descrição da estrutura geral do projeto

Cada um das pastas `server/src` e `client/src` contêm os ficheiros necessários para a compilação das aplicações `DS` e `User`, respetivamente. Após a execução do comando `make`, são criadas as pastas `server/obj` e `client/obj` para guardar todos os `.o` de cada uma das aplicações.

### Servidor
#### server/main.c:
    - Executa o loop principal do servidor
#### server/init_args_parsing.h
    - Contém as funções para validar os argumentos iniciais do programa.
#### server/connection.h:
    - Contém as funções para inicializar os endereços e os sockets do servidor.
#### server/tcp_handling.h:
    - Responsável pelo tratamento dos comandos enviados usando o protocolo TCP.
#### server/udp_handling.h:
    - Responsável pelo tratamento dos comandos enviados usando o protocolo UDP.
#### server/command_args_parsing.h:
    - Contém as funções para validar os argumentos dos comandos do protocolo.

#### server/operations.c/.h:
    - Contém as funções necessárias para manipular o sistema de ficheiros, este que é usado para manter o estado do servidor.

### Cliente
#### client/main.c:
    - Executa o loop principal do servidor.
#### client/init_args_parsing.h:
    - Contém as funções para validar os argumentos iniciais do programa.
#### client/interface.h:
    - Contém todas as mensagens que o cliente pode escrever na consola, bem como as palavras que o utilizador tem que escrever na consola para executar cada um dos comandos do protocolo.
#### client/commands.c/.h:
    - Responsável pelo envio de cada um dos comandos ao servidor.
#### client/commands_args_parsing.h
    - Contém as funções para validar os argumentos dos comandos do protocolo. A única diferença que tem em relação ao ficheiro correspondente no servidor é que não exige que os argumentos sejam dados com tamanho fixo, por exemplo, o user ID "00001" pode ser fornecido pelo utilizador como "1". 