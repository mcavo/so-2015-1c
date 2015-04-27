#include "../inc/fifo.h"

static void parse_to_send(message_t *msg, char *buffer);
static void parse_to_recive(message_t *msg, char *buffer);

void default_signal(){
	printf("atrapé SIGPIPE\n");
}

ipc_t *ipc_create(pid_t to) {
	//TODO: revisar que no hay que agregar más campos
	ipc_t *ipc = malloc(sizeof(ipc_t));
	ipc -> to = to;
	ipc -> from = getpid();
	ipc -> fifopath = get_fname(ipc->to);
	
	if (mknod(ipc -> fifopath, S_IFIFO | 0666, 0) == -1) {
		if(errno == EEXIST) {
			printf("the fifo already exists\n");
		} else {
			printf("error to create fifo\n");
			return NULL;	
		}
	}
	return ipc;
}

ipc_t *ipc_listen(pid_t to) {
	ipc_t *ipc = ipc_create(to);
	if( ipc == NULL) {
		return NULL;
	}
	signal(SIGPIPE, default_signal);
	printf("listen fifo path:%s\n", ipc->fifopath);
	return ipc;
}

ipc_t *ipc_connect(pid_t to) {
	ipc_t *ipc = ipc_create(to);
	if(ipc == NULL) {
		return NULL;
	}
	printf("connect fifo path:%s\n", ipc->fifopath);
	return ipc;
}

void ipc_close(ipc_t *ipc) {
	//TODO: check if it's needed to free something else.
	unlink(ipc -> fifopath);

	free(ipc -> fifopath);
	free(ipc);
}

void ipc_send(message_t *msg, ipc_t *ipc) {
	int fd = open(ipc -> fifopath, O_WRONLY);
	if(fd == ERROR){
		printf("error al abrir el archivo para enviar\n");
		//TODO: manejar el error
	}
	//TODO: ver si hacer falta checkear que se mande el mensaje completo.
	char* buffer = calloc(MESSAGE_SIZE, sizeof(char)*MESSAGE_SIZE);
	parse_to_send(msg, buffer);
	printf("voy a escribir: %s\n", buffer);
	printf("tamaño del buffer%d\n", sizeof(char)*MESSAGE_SIZE);
	int i =write(fd, buffer, sizeof(char)*MESSAGE_SIZE);
	printf("caracteres escritos: %d\n", i);
	printf("libero el buffer:%s\n", buffer);
	free(buffer);
	close(ipc -> fd);
}

message_t *ipc_read(ipc_t *ipc) {
	int fd = open(ipc -> fifopath, O_RDWR);
	if(fd == ERROR){
		printf("error al abrir el archivo para recibir\n");
		//TODO: manejar el error
		exit(1);
	}
	char *buffer = calloc(MESSAGE_SIZE + 1, sizeof(char));

	message_t *msg = malloc(sizeof(message_t));
	printf("entré en el read\n");
	int i = read(fd, buffer, sizeof(char)*MESSAGE_SIZE); /* garantee an atomic read */
	printf("caracteres leidos %d\n", i);
	parse_to_recive(msg, buffer);
	free(buffer);
	close(fd);
	return msg;
}

static void parse_to_recive(message_t *msg, char *buffer) { //TODO: arreglar esto y modularizarlo.
	char *length = calloc(LINE_LENGTH, sizeof(char));
	char *content = calloc(LINE_LENGTH, sizeof(char));
	char *from = calloc(LINE_LENGTH, sizeof(char));

	strncpy(length, buffer, LINE_LENGTH);
	strncpy(content, (buffer + LINE_LENGTH), LINE_LENGTH);
	strncpy(from, (buffer + 2 * LINE_LENGTH), LINE_LENGTH);

	/* these are where will be setted the formatted strings */
	char *length_short = calloc(LINE_LENGTH, sizeof(char));
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
}