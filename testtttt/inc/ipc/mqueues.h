#ifndef __MQUEUES_H__
#define __MQUEUES_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "../ipc.h"

typedef struct {
	uint16_t sender;
	uint16_t content_length;
	char content[];
} message_t;


typedef struct {
	uint16_t server_id;
	uint16_t id;
	message_t *message;
	char * addr;
} ipc_t;


ipc_t *ipc_listen(int pid);
ipc_t *ipc_connect(int pid);

ipc_t* ipc_open(char *root);
void ipc_close(ipc_t *ipc);

void ipc_send(ipc_t *ipc, uint16_t recipient, void *message, uint16_t size);
message_t* ipc_receive(ipc_t *ipc);

#endif
