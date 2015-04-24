#ifndef __MQUEUES_H__
#define __MQUEUES_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


typedef struct {
	uint16_t server_id;
	uint16_t id;
	message_t *message;
	char * addr;
} ipc_t;


ipc_t *ipc_listen(char *address);
ipc_t *ipc_connect(char *address);

ipc_t* ipc_open(char *root);
void ipc_close(ipc_t *ipc);

void ipc_send(ipc_t *ipc, uint16_t recipient, void *message, uint16_t size);
message_t* ipc_receive(ipc_t *ipc);

#endif
