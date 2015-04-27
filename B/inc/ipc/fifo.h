#ifndef __FIFO_H__
#define __FIFO_H__

#include <stdint.h>
#include <sys/stat.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include "ipc_parser.h"

#define SYSV_KEY_FIFO 1

typedef struct {
	//uint16_t id;
	//uint16_t server_id;
	pid_t from;
	pid_t to;
	int fd;
	char *fifopath;
	char *lockpath;
	int lock;
} ipc_t;

ipc_t *ipc_create(pid_t to);
ipc_t *ipc_listen(pid_t to);
ipc_t *ipc_connect(pid_t to);
void ipc_close(ipc_t *ipc);
void ipc_send(message_t *msg, ipc_t *ipc);
message_t *ipc_read(ipc_t *ipc);

//#endif