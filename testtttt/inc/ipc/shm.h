#ifndef __SHM_H__
#define __SHM_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define CLIENT_CAN_WRITE 1
#define SERVER_CAN_READ 2
#define CLIENT_CAN_READ 3
#define SERVER_CAN_WRITE 4

#define SIZE 100

typedef struct {
	uint16_t server_id;
	uint16_t id;
	message_t *shared_memory;
	char * addr;
} ipc_t;


ipc_t *ipc_listen(char *address);
ipc_t *ipc_connect(char *address);

ipc_t* ipc_open(char *root);
void ipc_close(ipc_t *ipc);

void ipc_send(ipc_t *ipc, uint16_t recipient, void *message, uint16_t size);
message_t* ipc_recive(ipc_t *ipc);

#endif