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
#include "../ipcParser.h"

#define SYSV_KEY_FIFO 1

typedef struct {
	pid_t id;
	pid_t server_id;
	int fd;
	char *fifopath;
} ipc_t;

ipc_t *ipc_create(pid_t to);
ipc_t *ipc_listen(pid_t to);
ipc_t *ipc_connect(pid_t to);
void ipc_close(ipc_t *ipc);
void ipc_send(ipc_t *ipc, uint16_t recipient, void *message, uint16_t len);
message_t *ipc_receive(ipc_t *ipc);

#endif