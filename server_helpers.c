#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
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

extern sig_atomic_t volatile CONTINUE_LISTENING;

void sig_handler(int signum)
{
  if (signum == SIGINT){
      CONTINUE_LISTENING = 0;
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

void serverChat(int connect_fd) {
    /* Declare buffer and variables to store return values from read() and write() */
    char buffer[MAX_INPUT];
    int numBytesWritten = 0, numBytesRead = 0;
    while (CONTINUE_CHATTING) {
        /* Initialize buffer with zeros */
        bzero(buffer, MAX_INPUT); 
        fprintf(stdout, "Your message: ");
        /* Get message from standard input and store in buffer */
        fgets(buffer, MAX_INPUT, stdin);
        /* Write to client */
        numBytesWritten = write(connect_fd, buffer, sizeof(buffer));
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
        /* If client sent '!QUIT', break out of loop and end chat */ 
        else if (strncmp("!QUIT", buffer, strlen("!QUIT")) == 0) { 
            fprintf(stdout, "Client Ended Connection\n");
            break; 
        }
        /* Display message from client */ 
        else {
            fprintf(stdout, "Client: %s\n", buffer);
        }
    }
}