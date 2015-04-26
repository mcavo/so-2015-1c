#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>

#define SERVER "../system_files/server-data.txt"
#define LINE_LENGTH 100 /* including '/n' */
#define SIZE_FILE 4 /* amount of messages allow */

#define START 0
#define SPACE 1
#define END 2

#define BOOL int
#define TRUE 1
#define FALSE 0

#define MAX_MESSAGES 5
#define MESSAGE_SIZE LINE_LENGTH * 3 /* 3 is the message line number */
typedef unsigned short uint16_t;

typedef struct {	
	pid_t from; /* client id. It will be used as the file name */
	pid_t to;
} ipc_t;

typedef struct {
	uint16_t length;
    char *content;
    pid_t from;
} message_t;

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