#ifndef __SOCKSTREAM_H__
#define __SOCKSTREAM_H__

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "../ipc.h"

#define SOCK_PATH "mysocket"//que lo pegue ahi


typedef struct {
	uint16_t sender;
	uint16_t content_len;
	char content[];
} message_t;

typedef struct {
	int id;
	int sock;
	uint16_t server_id; 
} ipc_t;

typedef struct{
	long mtype;
	message_t mtext;
}buf_t;


ipc_t *ipc_listen(int pid);
ipc_t *ipc_connect(int pid,int server_id);

ipc_t* ipc_open(int pid);
void ipc_close(ipc_t *ipc);

void ipc_send(ipc_t *ipc, uint16_t recipient, void *message, uint16_t len);
message_t* ipc_receive(ipc_t *ipc);

#endif
