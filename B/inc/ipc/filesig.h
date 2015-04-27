#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include "ipc_parser.h"

#define SERVER "../system_files/server-data.txt"
#define SIZE_FILE 4 /* amount of messages allow */

typedef struct {	
	pid_t from; /* client id. It will be used as the file name */
	pid_t to;
} ipc_t;

typedef struct {
	size_t head; /* start at 1 */
	size_t tail; /* start at 1 */
	BOOL read;
	BOOL write;
} header_t;

ipc_t *ipc_create(pid_t destination_process);
ipc_t *ipc_listen(pid_t destination_process);
ipc_t *ipc_connect(pid_t destination_process);
void ipc_close(ipc_t *ipc);
void ipc_send(message_t *msg, ipc_t *ipc);
message_t *ipc_read(ipc_t *ipc);
