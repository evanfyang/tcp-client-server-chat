#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h>
#include "server_helpers.h"

/* Define constants and global variables */
#define DEFAULT_PORT 8080
#define DEFAULT_IP_ADDR "127.0.0.1"

#define MIN_PORT_NUM 2000
#define MAX_PORT_NUM 65535

#define MAX_BACKLOG 5

sig_atomic_t volatile CONTINUE_LISTENING = 1;

int main(int argc, char *argv[])
{
    char *IP_ADDRESS;
    int PORT;

    /* When run with no arguments, use default IP Address and port */
    if (argc == 1) {  
	    IP_ADDRESS = DEFAULT_IP_ADDR;
        PORT = DEFAULT_PORT;
    }
    else if (argc == 2) {
        /* Check if IP Address is valid. If valid, use it and default port. */
        if (valid_ip(argv[1])) {
            IP_ADDRESS = argv[1];
            PORT = DEFAULT_PORT;
        }
        /* Prompt user to enter a valid IP Address */
        else {
            fprintf(stderr, "%s: Error: Invalid IP Address %s\n", argv[0], argv[1]);
            fprintf(stderr, "%s: IPv4 Format: [x.x.x.x], x = 0 to 255 (inclusive)\n", argv[0]);
	        exit(0);
        }
    }
    else if (argc == 3) {
        /* Check if IP Address is valid. If valid, use it and check if port number is valid. */
        if (valid_ip(argv[1])) {
            IP_ADDRESS = argv[1];
            /* If port number is valid, use it */
            if (only_digits(argv[2]) && atoi(argv[2]) > MIN_PORT_NUM && atoi(argv[2]) < MAX_PORT_NUM) {
                PORT = atoi(argv[2]);
            }
            /* Prompt user to valid Port Number */
            else {
                fprintf(stderr, "%s: Error: Invalid Port Number %s\n", argv[0], argv[2]);
                fprintf(stderr, "%s: Port Number Range: 2000 to 65535\n", argv[0]);
                exit(0);
            }
        }
        /* Prompt user to enter a valid IP Address */
        else {
            fprintf(stderr, "%s: Error: Invalid IP Address %s\n", argv[0], argv[1]);
            fprintf(stderr, "%s: IPv4 Format: [x.x.x.x], x = 0 to 255 (inclusive)\n", argv[0]);
            exit(0);
        }
    }
    /* Cannot call server with more than 2 command line arguments */ 
    else {
        fprintf(stderr, "Usage: %s [<IP_ADDRESS>] [<PORT_NUMBER>]\n", argv[0]);
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
    /* Specify network interface to bind with given IP Address and Port Number */ 
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(IP_ADDRESS);
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
        fprintf(stdout, "Waiting for Incoming Connection...\n");
    }
    /* Initialize variable to store client connection file descriptor */
    /* Get size of client_addr struct to use in accept */
    int connect_fd = 0;
    int client_len = sizeof(client_addr);
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
        serverChat(connect_fd);
        /* Close file descriptor used to communicate with client, display error message on failure */
        if (close(connect_fd) < 0) {
            perror("Error closing socket");
            exit(1);
        }
        /* Display success message if close() was successful */
        else {
            fprintf(stdout, "Client Ended Connection\n");
            fprintf(stdout, "Waiting for Incoming Connection...\n");
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