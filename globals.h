#include "stdio.h"
#include "stdlib.h"
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
int client[2];//pipes from client
int server[2];//pipes from server

pid_t serverID;
pid_t clientID;

#define BOARD_MIN 3
#define BOARD_MAX 9