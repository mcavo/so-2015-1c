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
#include "../ipcParser.h"

#define SOCK_PATH "mysocket"
#define MSG_SIZE 1024*10
#define MESSAGE_SIZE 500



typedef struct {
	int id;
	int sock;
	uint16_t server_id; 
} ipc_t;

typedef struct{
	long mtype;
	message_t mtext;
}buf_t;



#endif
