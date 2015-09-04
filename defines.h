#ifndef DEFINES_H
#define DEFINES_H

//Common file header
#include <unistd.h>
#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/epoll.h>
#include <netdb.h>
#include <fcntl.h>
#include <getopt.h>
//Standard header 
#include <string.h>
#include <errno.h>
#include <memory.h>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <iostream>
#include <vector>
using namespace std;

//Process Module header
#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <sys/stat.h>
#include <sys/uio.h>
//time
#include <sys/time.h>
#include <time.h>

#define MAX_PATH 1024
#define MAX_BUF	 1024

void start_server(struct in_addr local_addr, unsigned short port);

#endif // DEFINES_H
