#ifndef CLIENT_HELPERS_H
#define CLIENT_HELPERS_H

int only_digits(char *str);
int valid_ip(char *ip_addr);
void clientChat(int connect_fd);

#endif	/* CLIENT_HELPERS_H */