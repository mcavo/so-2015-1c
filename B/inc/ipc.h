#ifndef __IPC_H__
#define __IPC_H__

#include <stdint.h>
#include "ipc/mqueues.h"



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



#endif
