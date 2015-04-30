#ifndef __IPC_H__
#define __IPC_H__

#include <stdint.h>



#if defined(IPC_FILESIGN)
	#include "ipc/filesign.h"

#elif defined(IPC_SOCKETS)
	#include "ipc/sockets.h"

#elif defined(IPC_SHM)
	#include "ipc/shm.h"

#elif defined(IPC_MQUEUES)
	#include "ipc/mqueues.h"

#elif defined(IPC_FIFOS)
	#include "ipc/fifo.h"

#else
	#error No IPC selected (use make IPC=[FILESIGN|SOCKETS])

#endif


ipc_t *ipc_listen(int pid);
ipc_t *ipc_connect(int pid);

ipc_t* ipc_open(int pid);
void ipc_close(ipc_t *ipc);

void ipc_send(ipc_t *ipc, uint16_t recipient, void *message, uint16_t len);
message_t* ipc_receive(ipc_t *ipc);



#endif
