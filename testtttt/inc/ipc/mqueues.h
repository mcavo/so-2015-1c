#ifndef __MQUEUES_H__
#define __MQUEUES_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include "../ipc.h"

#define MSG_SIZE 1024*10

typedef struct {
	uint16_t sender;
	char *content;
} message_t;


typedef struct {
	int id;
} ipc_t;

typedef struct{
	long mtype;
	message_t mtext;
}buf_t;


ipc_t *ipc_listen(int pid);
ipc_t *ipc_connect(int pid);

ipc_t* ipc_open(int pid);
void ipc_close(ipc_t *ipc);

void ipc_send(ipc_t *ipc, message_t *message, int size);
message_t* ipc_receive(ipc_t *ipc);

#endif
