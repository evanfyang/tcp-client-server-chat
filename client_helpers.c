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

#define MIN_VALID_DIGIT '0'
#define MAX_VALID_DIGIT '9'

#define MIN_IPV4_NUM 0
#define MAX_IPV4_NUM 255

#define IPV4_NUM_DOTS 3

#define MAX_INPUT 256
#define CONTINUE_CHATTING 1

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
 
/* return 1 if IP Address is valid, return 0 otherwise. */
int valid_ip(char *ip_addr)
{
    int num_int, decimal_dots = 0;
    char *numbers;
 
    if (ip_addr == NULL) {
        return 0;
    }
 
    /* Split IP Address by "." */
    numbers = strtok(ip_addr, ".");
 
    if (numbers == NULL) {
        return 0;
    }
 
    while (numbers) {
        /* after parsing string, it must contain only digits */
        if (!only_digits(numbers)) {
            return 0;
        }
        /* convert number string to integer */
        num_int = atoi(numbers);
        /* check if the number is between 0 and 255 inclusive */
        if (num_int >= MIN_IPV4_NUM && num_int <= MAX_IPV4_NUM) {
            /* parse the remaining string */
            numbers = strtok(NULL, ".");
            /* incrememt decimal dot count */
            if (numbers != NULL) {
                decimal_dots++;
            }
        } 
        /* if number is not between 0 and 255 inclusive, return 0 */ 
        else {
            return 0;
        }
    }
 
    /* valid IP string must contain 3 dots */
    if (decimal_dots != IPV4_NUM_DOTS) {
        return 0;
    }
    /* valid IP Address */
    return 1;
}

void clientChat(int socket_fd) {
    /* Declare buffer and variables to store return values from read() and write() */
    char buffer[MAX_INPUT];
    int numBytesWritten = 0, numBytesRead = 0;
    while (CONTINUE_CHATTING) {
        /* Initialize buffer with zeros */
        bzero(buffer, MAX_INPUT);
        fprintf(stdout, "Waiting for Server...\n"); 
        /* Read from server */
        numBytesRead = read(socket_fd, buffer, sizeof(buffer));
        /* If write() fails, print error message and exit */
        if (numBytesRead < 0) {
            perror("Error on read");
            exit(1);
        }
        /* Server sends nothing, likely terminated by SIGINT */
        else if (numBytesRead == 0) {
            fprintf(stdout, "Server Terminated\n");
            fprintf(stdout, "Closing Connection...\n");
            break; 
        }
        /* Server exited by sending '!QUIT' */
        else if (strncmp("!QUIT", buffer, strlen("!QUIT")) == 0) {
            fprintf(stdout, "Server Terminated\n");
            fprintf(stdout, "Closing Connection...\n");
            break; 
        }
        /* Display message from server */ 
        else {
            fprintf(stdout, "Server: %s", buffer);
        }
        /* Reset buffer with zeros */
        bzero(buffer, MAX_INPUT); 
        fprintf(stdout, "Your message: ");
        /* Get message from standard input and store in buffer */
        fgets(buffer, MAX_INPUT, stdin);
        /* If client send '!QUIT', break out of loop and end chat */ 
        if (strncmp("!QUIT", buffer, strlen("!QUIT")) == 0) { 
            /* Write !QUIT to server */
            numBytesWritten = write(socket_fd, buffer, sizeof(buffer));
            fprintf(stdout, "Closing Connection...\n");
            break; 
        }
        /* Write to server */
        numBytesWritten = write(socket_fd, buffer, sizeof(buffer));
        /* If write() fails, print error message and exit */
        if (numBytesWritten < 0) {
            perror("Error on write");
            exit(1);
        }
        /* Display success message */ 
        else {
            fprintf(stdout, "Message Sent to Server\n");
        }
    }
}