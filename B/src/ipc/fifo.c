#include "../../inc/ipc/fifo.h"

static void parse_to_send(message_t *msg, char *buffer);
static void parse_to_recive(message_t *msg, char *buffer);

void default_signal(){
	printf("atrapé SIGPIPE\n");
}

ipc_t *ipc_create(pid_t to) {
	//TODO: revisar que no hay que agregar más campos
	ipc_t *ipc = malloc(sizeof(ipc_t));
	ipc -> server_id = to;
	printf("ipc server_id:%d\n", to);
	ipc -> id = getpid();
	printf("ipc id%d\n", getpid());
	ipc -> fifopath = get_fname(ipc->server_id);
	
	if (mknod(ipc -> fifopath, S_IFIFO | 0666, 0) == -1) {
		if(errno == EEXIST) {
			printf("the fifo already exists\n");
		} else {
			printf("error to create fifo\n");
			return NULL;	
		}
	}
	printf("estoy saliendo del create\n");
	return ipc;
}

ipc_t *ipc_listen(pid_t to) {
	ipc_t *ipc = ipc_create(to);
	if( ipc == NULL) {
		return NULL;
	}
	//signal(SIGPIPE, default_signal);
	printf("listen fifo path:%s\n", ipc->fifopath);
	printf("voy a abrir\n");
	int fd = open(ipc -> fifopath, O_RDONLY);
	printf("abrí el archivo\n");
	if(fd == ERROR){
		printf("error al abrir el archivo para recibir\n");
		//TODO: manejar el error
		exit(1);
	}
	ipc -> fd = fd;
	printf("listen fifo path:%s\n", ipc->fifopath);
	return ipc;
}

ipc_t *ipc_connect(pid_t to) {
	ipc_t *ipc = ipc_create(to);
	if(ipc == NULL) {
		return NULL;
	}
	printf("voy a abrir\n");
	int fd = open(ipc -> fifopath, O_WRONLY);
	printf("abrí el archivo\n");
	if(fd == ERROR){
		printf("error al abrir el archivo para enviar\n");
		//TODO: manejar el error
	}
	ipc -> fd =fd;
	printf("connect fifo path:%s\n", ipc->fifopath);
	return ipc;
}

void ipc_close(ipc_t *ipc) {
	//TODO: check if it's needed to free something else.
	close(ipc -> fd);
	unlink(ipc -> fifopath);
	free(ipc -> fifopath);
	free(ipc);
}

void ipc_send(ipc_t *ipc, uint16_t recipient, void *message, uint16_t len) {
	printf("send\n");
	message_t *msg = malloc(MESSAGE_SIZE); //TODO: el máximo debería ser más chico.
	msg -> sender = getpid();
	msg -> content_len = len;
	memcpy(msg->content, message, len);
	int i =write(ipc -> fd, msg, MESSAGE_SIZE);
	printf("caracteres escritos: %d\n", i);
	printf("libero el buffer:%s\n", msg->content);
	free(msg);
}

message_t *ipc_receive(ipc_t *ipc) {
	char *buffer = calloc(MESSAGE_SIZE, sizeof(char));

	message_t *msg = malloc(MESSAGE_SIZE);
	printf("entré en el read\n");
	int i = read(ipc -> fd, buffer, MESSAGE_SIZE); /* garantee an atomic read */
	printf("caracteres leidos %d\n", i);
	msg->content_len = ((uint16_t *)buffer)[1];
	msg->sender = ((uint16_t *)buffer)[0];
	msg = realloc(msg, sizeof(uint16_t)*2 + msg->content_len);
	memcpy(msg -> content, buffer + sizeof(uint16_t)*2, msg->content_len);
	free(buffer);
	return msg;
}
/*
static void parse_to_recive(message_t *msg, char *buffer) { //TODO: arreglar esto y modularizarlo.
	char *length = calloc(LINE_LENGTH, sizeof(char));
	char *content = calloc(LINE_LENGTH, sizeof(char));
	char *from = calloc(LINE_LENGTH, sizeof(char));

	strncpy(length, buffer, LINE_LENGTH);
	strncpy(content, (buffer + LINE_LENGTH), LINE_LENGTH);
	strncpy(from, (buffer + 2 * LINE_LENGTH), LINE_LENGTH);

	/* these are where will be setted the formatted strings */
/*	char *length_short = calloc(LINE_LENGTH, sizeof(char));
	char *content_short = calloc(LINE_LENGTH, sizeof(char));
	char *from_short = calloc(LINE_LENGTH, sizeof(char));	

	parse_string(length, length_short);
	parse_string(content, content_short);
	parse_string(from, from_short);

	msg->length = atoi(length_short);
	msg->content = content_short;
	msg->from = atoi(from_short);

	free(length);
	free(content);
	free(from);
	free(from_short);
	free(length_short);
}

static void parse_to_send(message_t *msg, char *buffer) {
	printf("entré a parse_to_sent\n");
	strcat(buffer, parse_line_int(msg -> length));
	strcat(buffer, parse_line_string(msg -> content));
	strcat(buffer, parse_line_int(msg->from));
	printf("terminé parse to send: %s\n", buffer);
}*/