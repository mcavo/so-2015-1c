#include "../../inc/ipc/filesig.h"
#include "../../inc/utils.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <libgen.h>

static BOOL flock_creat(int start, int end, int fd, int type);

static BOOL flock_read(int start, int end, int fd);

static BOOL flock_write(int start, int end, int fd);

static BOOL flock_unlock(int start, int end, int fd);

static int fsize(int fd);

ipc_t *ipc_create(char *root) {
	ipc_t *ipc = (ipc_t*) malloc(sizeof(ipc_t));
	ipc->root = root;
	mkdir(root, 0777);
	install_signal_handler();
	return ipc;
}

ipc_t *ipc_listen(char *dir) {
	ipc_t *ipc = ipc_create(strdup(dir));
	ipc->server_id = ipc->id = getpid();
	return ipc;
}

ipc_t *ipc_connect(char *file) {
	int ret = access(file, F_OK | R_OK);
	check(ret != -1, "Cannot open %s\n", file);
	ipc_t *ipc = ipc_create(fpath(file));
	ipc->id = getpid();
	ipc->server_id = atoi(fname(file));
	return ipc;
}

void ipc_close(ipc_t* ipc) {
	char path[250];
	sprintf(path, "%s/%d", ipc->root, ipc->id);
	unlink(path);
	free(ipc->root);
	free(ipc);
}

void ipc_send(ipc_t* ipc, uint16_t recipient, void *content, uint16_t length) {
char path[250];
sprintf(path, "%s/%d", ipc->root, recipient);
FILE *inbox = fopen(path, "a");
check(inbox != NULL, "Cannot open %s\n", path);
fwlock(inbox);
fwrite(content, length, 1, inbox);
/* The content is written *before* the header. The file will then be
* read from the end. This allows recv() to read a single message and
* truncate the file (which can't be done from the beginning).
*/
fwrite(&(ipc->id), sizeof(ipc->id), 1, inbox);
fwrite(&length, sizeof(length), 1, inbox);
funlock(inbox);
fclose(inbox);
/* Notify receipient: */
kill(recipient, SIGUSR1);
}
message_t* ipc_recv(ipc_t* ipc) {
	char path[250];
	sprintf(path, "%s/%d", ipc->root, ipc->id);
	FILE *inbox = fopen(path, "a+");
	check(inbox != NULL, "Cannot open %s\n", path);
	size_t size;
	/* Check if the file has messages, wait if it doesn't: */
	while ((size = fsize(inbox)) == 0)
	pause();
	frlock(inbox);
	/* Read header (located at the end of the file): */
	uint16_t sender;
	uint16_t content_length;
	fseek(inbox, -sizeof(message_t), SEEK_END);
	fread(&sender, sizeof(sender), 1, inbox);
	fread(&content_length, sizeof(content_length), 1, inbox);
	/* Alloc and read content (located before header): */
	size_t message_size = sizeof(message_t) + content_length;
	fseek(inbox, -message_size, SEEK_END);
	message_t* msg = (message_t*) malloc(message_size);
	fread(msg->content, content_length, 1, inbox);
	msg->sender = sender;
	msg->content_length = content_length;
	/* Remove this message from the inbox: */
	ftruncate(fileno(inbox), size - message_size);
	funlock(inbox);
	fclose(inbox);
	return msg;
}

static BOOL flock_creat(int start, int end, int fd, int type) {
    struct flock fl;

    fl.l_type = type;
    fl.l_whence = SEEK_SET;
    fl.l_start = start;
    fl.l_len = end;
    fl.l_pid = getpid();

    if(fcntl(fd, F_SETLK, &fl) == -1) { //pensar bien los casos
        printf("Trying to connect to database. Wait please...\n");
        if (fcntl(fd, F_SETLKW, &fl) == -1) {
            return FALSE;
        }
    }
    return TRUE;
}

static BOOL flock_read(int start, int end, int fd) {
     return flock_creat(start, end, fd, F_RDLCK);
}

static BOOL flock_write(int start, int end, int fd) {
    return flock_creat(start, end, fd, F_WRLCK);
}

static BOOL flock_unlock(int start, int end, int fd) {
    return flock_creat(start, end, fd, F_UNLCK);
}

static int fsize(int fd) {
	struct stat st;
	if (fstat(fd, &st) != 0)
		return 0;
	return st.st_size;
}
