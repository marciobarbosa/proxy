/* Developed by Marcio Brito Barbosa */

#ifndef CONNECTION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h> 
#include <sys/stat.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#include "../hdr/util.h"

#define MAX_SIZE 4096
#define MAX_LINE 4096

int setupServer(int port_number);
void startServer(int server_sock, filterList *begin);

#endif