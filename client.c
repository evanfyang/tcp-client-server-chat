#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <netdb.h> 
#include <arpa/inet.h>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h>
#include "client_helpers.h"

/* Define constants */
#define MIN_PORT_NUM 2000
#define MAX_PORT_NUM 65535

int main(int argc, char *argv[])
{
    char *IP_ADDRESS;
    int PORT;

    /* Must call client with 3 command line arguments */ 
    if (argc < 3 || argc > 3) {
        fprintf(stderr, "Usage: %s [<IP_ADDRESS>] [<PORT_NUMBER>]\n", argv[0]);
	    exit(1);
    }
    else {
        /* Check if IP Address is valid. If valid, check if port number is valid */
        if (valid_ip(argv[1])) {
            IP_ADDRESS = argv[1];
            /* Check if Port Number is valid */
            if (only_digits(argv[2])) {
                PORT = atoi(argv[2]);
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
        /* Prompt user to enter a valid IP Address */
        else {
            fprintf(stderr, "%s: Error: Invalid IP Address %s\n", argv[0], argv[1]);
            fprintf(stderr, "%s: IPv4 Format: [x.x.x.x], x = 0 to 255 (inclusive)\n", argv[0]);
            exit(1);
        }
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
    /* Declare sockaddr_in structs for server */
    struct sockaddr_in server_addr;
    /* Initialize server_addr sockaddr_in struct to zeros */
    bzero(&server_addr, sizeof(server_addr));
    /* Get host information from IP Address and store in a hostent struct */ 
    struct hostent *server;
    server = gethostbyname(IP_ADDRESS);
    /* Cannot find host with specified IP Address */
    if (server == NULL) {
        fprintf(stderr,"Error: The host %s does not exist!\n", IP_ADDRESS);
        exit(1);
    }
    /* Specify network interface to connect with given IP Address and Port Number */ 
    server_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&server_addr.sin_addr.s_addr, server->h_length);
    // server_addr.sin_addr.s_addr = inet_addr(IP_ADDRESS);
    server_addr.sin_port = htons(PORT);
    /* Connect client socket to server socket, display error message on failure */ 
    fprintf(stdout, "Connecting to Server...\n"); 
    if (connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error connecting to server socket");
        exit(1);
    }
    /* Display success message if connect() was succesful */
    else {
        fprintf(stdout, "Connected to Server\n");
    }
    /* Start chat with server using client socket file descriptor */
    clientChat(socket_fd);

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