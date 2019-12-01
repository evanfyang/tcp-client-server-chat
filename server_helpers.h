#ifndef SERVER_HELPERS_H
#define SERVER_HELPERS_H

int only_digits(char *str);
int valid_ip(char *ip_addr);
void sig_handler(int signum);
void serverChat(int connect_fd);

#endif	/* SERVER_HELPERS_H */