#ifndef __IPC_H__
#define __IPC_H__

#include <stdint.h>

typedef struct {
	uint16_t sender;
	uint16_t content_length;
	char content[];
} message_t;


#if defined(IPC_FILESIGN)
	#include "ipc/filesign.h"

#elif defined(IPC_SOCKETS)
	#include "ipc/sockets.h"

#elif defined(IPC_SHMEM)
	#include "ipc/shm.h"

#elif defined(IPC_MQUEUES)
	#include "ipc/mqueues.h"

#elif defined(IPC_FIFOS)
	#include "ipc/fifo.h"

#else
	#error No IPC selected (use make IPC=[FILESIGN|SOCKETS])

#endif

ipc_t *ipc_listen(char *address);
ipc_t *ipc_connect(char *address);

ipc_t* ipc_open(char *root);
void ipc_close(ipc_t *ipc);

void ipc_send(ipc_t *ipc, uint16_t recipient, void *message, uint16_t size);
message_t* ipc_receive(ipc_t *ipc);


#endif
