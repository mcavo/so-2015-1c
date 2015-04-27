#include "../inc/ipc.h"

#if defined(IPC_FILESIGN)
	#include "ipc/filesign.c"

#elif defined(IPC_SOCKETS)
	#include "ipc/sockStream.c"

#elif defined(IPC_SHMEM)
	#include "ipc/shm.c"

#elif defined(IPC_MQUEUES)
	#include "ipc/mqueues.c"

#elif defined(IPC_FIFOS)
	#include "ipc/fifo.c"

#endif

