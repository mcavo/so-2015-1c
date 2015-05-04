#include "../../inc/ipc/fifo.h"

void default_signal(){
	printf("Se cerró el servidor. Vuelva a intentar más tarde\n");
	exit(1);
}

ipc_t *ipc_create(pid_t to) {
	ipc_t *ipc = malloc(sizeof(ipc_t));
	ipc -> server_id = to;
	ipc -> id = getpid();
	ipc -> fifopath = get_fname(ipc->server_id);
	printf("%s\n",ipc->fifopath);
	if (mkfifo(ipc -> fifopath, 0777) == -1) {
		if(errno != EEXIST) {
			perror("create");
			fprintf(stderr, "Ocurrio el error %s en ipc_create\n",strerror(errno));		
			exit(1);	
		}
	}
	return ipc;
}

ipc_t *ipc_listen(pid_t to) {
	ipc_t *ipc = ipc_create(to);
	signal(SIGPIPE, default_signal);
	int fd = open(ipc -> fifopath, O_RDONLY);
	if(fd == ERROR){
		perror("fifo listen");
		fprintf(stderr, "Ocurrio el error %s en ipc_listen\n",strerror(errno));		
		exit(1);
	}
	ipc -> fd = fd;
	return ipc;
}

ipc_t *ipc_connect(pid_t to) {
	ipc_t *ipc = ipc_create(to);
	int fd = open(ipc -> fifopath, O_WRONLY);
	if(fd == ERROR){
		perror("connect");
		fprintf(stderr, "Ocurrio el error %s en ipc_connect\n",strerror(errno));		
		exit(1);
	}
	ipc -> fd =fd;
	return ipc;
}

void ipc_close(ipc_t *ipc) {
	close(ipc -> fd);
	unlink(ipc -> fifopath);
	free(ipc -> fifopath);
	free(ipc);
}

void ipc_send(ipc_t *ipc, uint16_t recipient, void *message, uint16_t len) {
	message_t *msg = calloc(MESSAGE_SIZE, sizeof(char));
	msg -> sender = getpid();
	msg -> content_len = len;
	memcpy(msg->content, message, len);
	write(ipc -> fd, msg, MESSAGE_SIZE);
	free(msg);
}

message_t *ipc_receive(ipc_t *ipc) {
	char *buffer = calloc(MESSAGE_SIZE, sizeof(char));

	message_t *msg = malloc(MESSAGE_SIZE);
	if(read(ipc -> fd, buffer, MESSAGE_SIZE) == 0) {
		close(ipc -> fd);
		ipc -> fd = open(ipc -> fifopath, O_RDONLY);
	}
	msg->content_len = ((uint16_t *)buffer)[1];
	msg->sender = ((uint16_t *)buffer)[0];
	msg = realloc(msg, sizeof(uint16_t)*2 + msg->content_len);
	memcpy(msg -> content, buffer + sizeof(uint16_t)*2, msg->content_len);
	free(buffer);
	return msg;
}
