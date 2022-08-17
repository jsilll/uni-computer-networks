Centralized Messaging Server and Client
===
Brief Description of the Project.

Commands
===
###### The commands related to the registration of a new user ID are:
- reg UID pass – following this command the User application sends a
message to the DS server, using the UDP protocol, asking to register a new user,
sending its identification UID and a selected password pass.
The result of the DS registration request should be displayed.
- unregister UID pass or unr UID pass – the User application sends
a message to the DS server, using the UDP protocol, asking to unregister the
user with identification UID and password pass. The DS server should
unsubscribe this user from all groups in which it was subscribed. The result of
the unregister request should be displayed.

###### The commands related to user identification and session termination are:
- login UID pass – with this command the User application sends a
message in UDP to the DS to validate the user credentials: UID and pass. The
result of the DS validation should be displayed to the user.
The User application memorizes the UID and pass in usage.
- logout – the User application (locally) forgets the credentials of the
previously logged in user. A new login command, with different credentials, can
then be issued.
- showuid or su – following this command the User application locally
displays the UID of the user that is logged in.
- exit – the User application terminates, after making that all TCP connections
are closed.

###### The commands related to group management are listed below.
- groups or gl – following this command the User application sends the DS
server a message in UDP asking for the list of available groups. The reply
should be displayed as a list of group IDs (GID), group names (GName) and the
number MID of the last message available for each group.

###### These following group management commands can only be issued after a user has
logged in:
- subscribe GID GName or s GID GName – following this command the
User application sends the DS server a message in UDP, including the user’s
UID, asking to subscribe the desired group, identified by its GID and GName. If
GID = 0 this corresponds to a request to create and subscribe to a new group
named GName. The confirmation of successful subscription (or not) should be
displayed.
- unsubscribe GID or u GID – following this command the User
application sends the DS server a message in UDP, including the user’s UID,
asking to unsubscribe group GID. The confirmation of success (or not) should
be displayed.
- my_groups or mgl – following this command the User application sends
the DS server a message in UDP, including the user’s UID, asking the list of
groups to which this user has already subscribed. The reply should be displayed
as a list of group IDs, group names and the number of the last message available.
- select GID or sag GID – following this command the User application
locally memorizes GID as the ID of the active group. Subsequent ulist, post
and retrieve messaging commands refer to this GID.
- showgid or sg – following this command the User application locally
displays the GID of the selected group.
- ulist or ul – following this command the User application sends the DS
server a message in TCP asking for the list of user UIDs that are subscribed to
the currently subscribed group GID.
The commands related to messaging are listed below. These commands can only be
issued after a user has logged in and an active group GID has been selected.
- post “text” [Fname] – following this command the User establishes a
TCP session with the DS server and sends a message containing text (between
“ “), and possibly also a file with name Fname.
The confirmation of success (or not) should be displayed, including the posted
message’s ID MID. The TCP connection is then closed.
- retrieve MID or r MID – following this command the User establishes a
TCP session with the DS server and sends a message asking to receive up to 20
messages, starting with the one with identifier MID, for the active group GID.

Compiling
===
```
cd computer-networks-project/
make
```

Server Source Files
===

#### server/main.c:
- Executes the server's main loop.
#### server/init_args_parsing.h
- Contains the necessary functions to validate the program's init arguments.
#### server/connection.h:
- Contains the necessary functions as funções to initialize the server's addresses and sockets.
#### server/tcp_handling.h:
- Responsible for handling commands sent using the TCP protocol.
#### server/udp_handling.h:
- Responsible for handling commands sent using the UDP protocol.
#### server/command_args_parsing.h:
- Contains the necessary functions to validate the arguents for each command.
#### server/operations.c/.h:
- Contains the necessary functions to manipulate the file system, in order to maintain the server's state.

Client Source Files
===

#### client/main.c:
- Executes the client's main loop.
#### client/init_args_parsing.h:
- Contains the necessary functions to validate the program's init arguments.
#### client/interface.h:
- Contains all the messages that the client application may display on the terminal as well as all the command keywords the user may use to interact with the server.  
#### client/commands.c/.h:
- Responsible for sending each command to the server (marshalling).
#### client/commands_args_parsing.h
- Contains the necessary functions to validate the arguents for each command. The verification rules are less strict than on the server for a better user experience, for example: the user ID "00001" may be writter by the user as "1". 
