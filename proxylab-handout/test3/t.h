#ifndef T_H
#define T_H

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <stdlib.h>

#define MAXLINE 8192
#define LISTENQ 1024

extern int open_clientfd(char *hostname,char *port);
extern int open_listenfd(char *port);

#endif

