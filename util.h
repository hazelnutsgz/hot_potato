#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <iostream>
#include <vector>
#include <string>

const int IP_STRING_SIZE = 50;

int listen_step(const char* port);
int connect_step(const char *hostname, const char* port);