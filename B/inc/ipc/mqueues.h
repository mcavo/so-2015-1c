#ifndef __MQUEUES_H__
#define __MQUEUES_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

#define MSG_SIZE 1024*10

typedef struct {
	uint16_t sender;
	uint16_t content_len;
	char content[];
} message_t;

typedef struct {
	int id;
	uint16_t server_id; 
	int queue;
} ipc_t;

typedef struct{
	long mtype;
	message_t mtext;
}buf_t;


#endif
