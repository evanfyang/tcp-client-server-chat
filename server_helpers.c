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

#define MIN_VALID_DIGIT '0'
#define MAX_VALID_DIGIT '9'

#define MIN_IPV4_NUM 0
#define MAX_IPV4_NUM 255

#define IPV4_NUM_DOTS 3

#define MAX_INPUT 256
#define CONTINUE_CHATTING 1

void sig_handler(int signum)
{
    if (signum == SIGINT){
        fprintf(stdout, "\nTerminating Server...\n");
        fprintf(stdout, "Goodbye!\n");
        exit(0);
    }
}

/* return 1 if string contain only digits, return 0 otherwise */
int only_digits(char *str)
{
    while (*str) {
        if (*str >= MIN_VALID_DIGIT && *str <= MAX_VALID_DIGIT) {
            ++str;
        }
        else {
            return 0;
        }
    }
    return 1;
}
 
int serverChat(int connect_fd) {
    /* Declare buffer and variables to store return values from read() and write() */
    char buffer[MAX_INPUT];
    int quitServer = 0;
    int numBytesWritten = 0, numBytesRead = 0;
    while (CONTINUE_CHATTING) {
        /* Initialize buffer with zeros */
        bzero(buffer, MAX_INPUT); 
        fprintf(stdout, "Your message: ");
        /* Get message from standard input and store in buffer */
        fgets(buffer, MAX_INPUT, stdin);
        /* Write to client */
        numBytesWritten = write(connect_fd, buffer, sizeof(buffer));
        if (strncmp("!QUIT", buffer, strlen("!QUIT")) == 0) { 
            quitServer = 1;
            break; 
        }
        /* If write() fails, print error message and exit */
        if (numBytesWritten < 0) {
            perror("Error on write");
            exit(1);
        }
        /* Display success message */ 
        else {
            fprintf(stdout, "Message Sent to Client\n");
        }
        fprintf(stdout, "Waiting for Client...\n");
        /* Reset buffer with zeros */
        bzero(buffer, MAX_INPUT); 
        /* Read from client */
        numBytesRead = read(connect_fd, buffer, sizeof(buffer));
        /* If write() fails, print error message and exit */
        if (numBytesRead < 0) {
            perror("Error on read");
            exit(1);
        }
        /* Client sends nothing, likely terminated by SIGINT */
        else if (numBytesRead == 0) {
            fprintf(stdout, "Client Terminated\n");
            break; 
        }
        /* If client sent '!QUIT', break out of loop and end chat */ 
        else if (strncmp("!QUIT", buffer, strlen("!QUIT")) == 0) { 
            fprintf(stdout, "Client Ended Connection\n");
            break; 
        }
        /* Display message from client */ 
        else {
            fprintf(stdout, "Client: %s", buffer);
        }
    }
    return quitServer;
}