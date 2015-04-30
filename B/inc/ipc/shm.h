#ifndef __SHM_H__
#define __SHM_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>

#include "../utils.h"

#define CLIENT_CAN_WRITE 1
#define SERVER_CAN_READ	 2
#define SERVER_CAN_WRITE 1
#define CLIENT_CAN_READ  2

#define SIZE 100

typedef struct {
	uint16_t sender;
	uint16_t content_len;
	char content[];
} message_t;

typedef struct {
	uint16_t id;
	uint16_t server_id;
	uint16_t shmid;
	message_t * shm;
} ipc_t;

#endif
