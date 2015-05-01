#ifndef __FILESIG_H__
#define __FILESIG_H__

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include "../ipcParser.h"

#define SERVER "../system_files/server-data.txt"
#define SIZE_FILE 4 /* amount of messages allow */

typedef struct {	
	pid_t id; /* client id. It will be used as the file name */
	pid_t server_id;
	int fd;
} ipc_t;

typedef struct {
	size_t head; /* start at 0 */
	size_t tail; /* start at 0 */
	// BOOL read;
	// BOOL write;
} header_t;

ipc_t *ipc_create(pid_t destination_process);
ipc_t *ipc_listen(pid_t destination_process);
ipc_t *ipc_connect(pid_t destination_process);
void ipc_close(ipc_t *ipc);
void ipc_send(ipc_t *ipc, uint16_t recipient, void *message, uint16_t len);
message_t *ipc_receive(ipc_t *ipc);

#endif