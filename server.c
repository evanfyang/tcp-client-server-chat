#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h>
#include "server_helpers.h"

/* Define constants and global variables */
#define DEFAULT_PORT 8080

#define MIN_PORT_NUM 2000
#define MAX_PORT_NUM 65535

#define MAX_BACKLOG 5

#define CONTINUE_LISTENING 1

int main(int argc, char *argv[])
{
    int PORT;

    /* When run with no arguments, run server using default port */
    if (argc == 1) {  
        PORT = DEFAULT_PORT;
    }
    else if (argc == 2) {
        /* Check if Port Number is valid */
        if (only_digits(argv[1])) {
            PORT = atoi(argv[1]);
            /* Check if Port Number is in the correct range. If not, exit */
            if (PORT < MIN_PORT_NUM || PORT > MAX_PORT_NUM) {
                fprintf(stderr, "%s: Error: Invalid Port Number %s\n", argv[0], argv[1]);
                fprintf(stderr, "%s: Port Number Range: 2000 to 65535\n", argv[0]);
                exit(1);
            }
        }
        /* Otherwise, prompt user to enter a valid Port Number */
        else {
            fprintf(stderr, "%s: Error: Invalid Port Number %s\n", argv[0], argv[1]);
            fprintf(stderr, "%s: Port Number Format: [xxxxx], x = 0 to 9 (inclusive)\n", argv[0]);
	        exit(1);
        }
    }
    /* Cannot call server with more than 1 command line argument */ 
    else {
        fprintf(stderr, "Usage: %s [<PORT_NUMBER>]\n", argv[0]);
	    exit(1);
    }

    /* Create new socket and store returned file descriptor */
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0); 
    /* Socket creation failed, display error message and exit */
    if (socket_fd < 0) {
        perror("Error creating socket"); 
        exit(1);
    }
    /* Otherwise, display success message */
    else {
        fprintf(stderr, "Socket Created\n");
    }
    /* Declare sockaddr_in structs for server and client */
    struct sockaddr_in server_addr, client_addr;
    /* Initialize server_addr sockaddr_in struct to zeros */
    bzero(&server_addr, sizeof(server_addr));
    /* Specify network interface to bind with the IP Address of the machine 
       running the server and the specified the Port Number */ 
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; /* Gets IP Address of machine */
    server_addr.sin_port = htons(PORT);
    /* Bind socket to the specified address, display error message on failure */
    if (bind(socket_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        perror("Error binding socket to address");
        exit(1);
    }
    /* Display success message on successful bind */
    else {
        fprintf(stdout, "Bind Completed\n");
    }
    /* Listen on the socket for connections, display error message on failure */
    if (listen(socket_fd, MAX_BACKLOG) < 0) {
        perror("Error listening for connections on socket");
        exit(1);
    }
    /* If listen does not fail, display success message */
    else {
        fprintf(stdout, "Waiting for Incoming Connection (Ctrl+C to Quit) ...\n");
    }
    /* Initialize variable to store client connection file descriptor */
    /* Get size of client_addr struct to use in accept */
    int connect_fd = 0;
    int quitServer = 0;
    socklen_t client_len = sizeof(client_addr);
    /* Call signal handler to handle keyboard interrupts to exit loop */
    signal(SIGINT, &sig_handler);
    /* Continue listening for client connections until keyboard interrupt */
    while (CONTINUE_LISTENING) {
        /* Accept connection from client, display error message on failure */
        connect_fd = accept(socket_fd, (struct sockaddr *) &client_addr, &client_len);
        if (connect_fd < 0) {
            perror("Error accepting client connection");
            exit(1);
        }
        /* Display success message if accept() was succesful */
        else {
            fprintf(stdout, "Connection Created with Client\n");
        }
        /* Chat with client using the file descriptor returned from accept() */
        quitServer = serverChat(connect_fd);
        /* Close file descriptor used to communicate with client, display error message on failure */
        if (close(connect_fd) < 0) {
            perror("Error closing socket");
            exit(1);
        }
        /* Display success message if close() was successful */
        else {
            fprintf(stdout, "Client Ended Connection\n");
            fprintf(stdout, "Waiting for Incoming Connection (Ctrl+C to Quit) ...\n");
        }
        if (quitServer) {
            fprintf(stdout, "Terminating Server...\n");
            break;
        }
    }
    /* Close socket, display error message on failure */
    if (close(socket_fd) < 0) {
        perror("Error closing socket");
        exit(1);
    }
    /* Successfully closed socket */
    else {
        fprintf(stdout, "Goodbye!\n");
    }
    return 0;
}