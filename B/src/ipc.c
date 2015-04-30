#include "../inc/ipc.h"

#if defined(IPC_FILESIGN)
	#include "ipc/filesign.c"

#elif defined(IPC_SOCKETS)
	#include "ipc/sockets.c"

#elif defined(IPC_SHM)
	#include "ipc/shm.c"

#elif defined(IPC_MQUEUES)
	#include "ipc/mqueues.c"

#elif defined(IPC_FIFOS)
	#include "ipc/fifo.c"

#endif


