#ifndef __IPC_H__
#define __IPC_H__

#include <stdint.h>

typedef struct {
	uint16_t sender;
	uint16_t content_length;
	char content[];
} message_t;


#if defined(IPC_FILESIG)
	#include "ipc/filesig.h"

#elif defined(IPC_SOCKETS)
	#include "ipc/sockets.h"

#elif defined(IPC_SHMEM)
	#include "ipc/shmem.h"

#elif defined(IPC_MQUEUES)
	#include "ipc/mqueues.h"

#elif defined(IPC_FIFOS)
	#include "ipc/fifo.h"

#else
	#error No IPC selected (use make IPC=[FILESIG|SOCKETS])

#endif
