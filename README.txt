AUTHOR: Evan Yang
EMAIL: evan.yang@uky.edu
DATE: 12/15/2019
COURSE: CS 371-001
Project: Programming Project - TCP Server-Client Chat

CONTENTS:

README.txt: This file.

server.c: implementation of the server program, specifically for creating the 
connection between the server and client.

server_helpers.c: implements all helper functions for the server program, 
including chat functionality for the server.

server_helpers.h: header file for server_helpers.c.

client.c: implementation of the client program, specifically for creating the 
connection between the client and server.

client_helpers.c: implements all helper functions for the client program, 
including chat functionality for the client.

client_helpers.h: header file for client_helpers.c.

RUNNING:

In the terminal, change the working directory to the one containing all of the
files listed under CONTENTS. Next, type the following command to compile each 
program:

gcc -o server server.c server_helpers.c
gcc -o client client.c client_helpers.c

To run the server, type the following:

./server [<PORT_NUMBER>]

Where [<PORT_NUMBER>] is the port number that you wish to run the server on.
If you run the server without providing a port number, i.e.:

./server

The server will default to port 8080.

To run the client, type the following:

./client [<IP_ADDRESS>] [<PORT_NUMBER>]

Where [<IP_ADDRESS>] is the IP Address of the machine running the server and 
[<PORT_NUMBER>] is the port number that the server is listening on. You must
provide both to run the client program!

IMPLEMENTATION NOTES:

As described under CONTENTS, socket creation was implemented in server.c and
client.c while chat functionalities were delegated to their respective 
*_helpers.c files. To terminate the client, the person on the client side can 
type '!QUIT' during chatting to terminate and the server will close the 
connection with the current client and continues listening. To terminate the 
server, the person on the server side can type '!QUIT' during chatting as well 
to terminate the server, which also terminates the current client connected to
the server. The server can also be terminated with a SIGINT from the keyboard 
(ctrl+c), which terminates the server if it is listening or chatting with the 
client. If the server is chatting with a client when it recieves a SIGINT, the
client will terminate as well. When the client is terminated by SIGINT, the 
server goes back to listening for another client connection.  

LIMITATIONS:

There is a maximum length that is imposed on input lines using fgets(). This 
maximum length is defined with a length of 256 (MAX_INPUT). This limitation is 
imposed on input lines to avoid buffer overflow attacks.

The server can only run on the IP address of the machine that is running the
server program. To connect with the server, the client must specify the 
IP Address of the server as well as the port number that the server is listening 
on for the client. For both the server program and client program to connect with
each other, both machines running the server and client must be connected to 
the same network. 

There were no failing test cases, memory leaks, or other memory errors that were
detected during the testing process. Further testing outside of provided test
cases did not detect any errors.

REFERENCES:

The following websites were used as references for various parts of the project:

https://www.cs.rpi.edu/~moorthy/Courses/os98/Pgms/socket.html
https://pubs.opengroup.org/onlinepubs/7908799/xns/syssocket.h.html
https://www.includehelp.com/c-programs/check-string-is-valid-ipv4-address-or-not.aspx
http://www.cplusplus.com/reference/cstring/strtok/
https://stackoverflow.com/questions/13010531/leaving-an-infinite-while-loop-in-c
https://stackoverflow.com/questions/6792930/how-do-i-share-a-global-variable-between-c-files/6792967
https://stackoverflow.com/questions/13465179/c-making-volatile-sig-atomic-t-global-in-header-file

Implementation of the main routine for server.c and client.c were based on the
server and client implmenetation by Robert Ingalls from the first link. 