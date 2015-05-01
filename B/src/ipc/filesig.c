#include "../../inc/ipc/filesig.h"

static BOOL flock_creat(int fd, int type);
static BOOL flock_write(int fd);
static BOOL flock_unlock(int fd);
static int open_file(ipc_t *ipc);
static void close_file(int fd);
static void read_message(message_t *msg, int fd);
static void write_message(message_t *msg, int fd);
static header_t *read_header(int fd);
static void write_header(header_t *header, int fd);

static void signal_handler(int signum) {
	/* it is supose to be empty */
}

static void install_signal_handler() {
	struct sigaction handler = {
		.sa_flags = 0,
		.sa_handler = signal_handler
	};
	sigaction(SIGUSR1, &handler, NULL);
}

ipc_t *ipc_create(pid_t destination_process) {
	ipc_t *ipc = (ipc_t*) malloc(sizeof(ipc_t));
	ipc -> server_id = destination_process;
	ipc -> id = getpid();
	install_signal_handler();
	int fd = open_file(ipc);
	if (fd == ERROR) {
		return NULL;
	}
	ipc -> fd = fd;
	return ipc;
}

ipc_t *ipc_listen(pid_t destination_process) {
	return ipc_create(destination_process);
}
/* recibe el nombre de un archivo */
ipc_t *ipc_connect(pid_t destination_process) {
	return ipc_create(destination_process);
}
/* close ipc destination */
void ipc_close(ipc_t *ipc) {
	close_file(ipc -> fd);
	//unlink(path);
	free(ipc);
}
/* send the message to the ipc destination */
void ipc_send(ipc_t *ipc, uint16_t recipient, void *message, uint16_t len) {
	while(!flock_write(ipc -> fd))
		;
	message_t *msg = calloc(MESSAGE_SIZE, sizeof(char)); //TODO: el máximo debería ser más chico.
	msg -> sender = getpid();
	msg -> content_len = len;
	memcpy(msg->content, message, len);
	write_message(msg, ipc -> fd);
	/* send signal to weak up de destination process */
	free(msg);

	kill(ipc -> server_id, SIGUSR1);
}
/* read and lock the file craeted with the ipc information */
message_t *ipc_receive(ipc_t *ipc) {
	while(!flock_write(ipc -> fd))
		;
	message_t *msg = (message_t*) calloc(1, MESSAGE_SIZE);
	read_message(msg, ipc -> fd);
	return msg;
}

static int open_file(ipc_t *ipc) {
	char* fname = get_fname(ipc -> server_id);
	int fd = open(fname, O_RDWR | O_CREAT, 0666);
	if (fd == ERROR) {
		return ERROR; //TODO: cómo manejar los errores en ipc??
	}
	return fd;
}

static void close_file(int fd) {
	flock_unlock(fd);
	close(fd);
}

//TODO: no hay que liberar el fl??
static BOOL flock_creat(int fd, int type) {
	struct flock fl;
	fl.l_type = type;
	fl.l_whence = SEEK_SET;
	fl.l_start = 0;
	fl.l_len = 0;
	fl.l_pid = getpid();
	if (fcntl(fd , F_SETLKW, &fl) == -1) {
		return FALSE;
	}
	return TRUE;
}

static BOOL flock_write(int fd) {
	return flock_creat(fd, F_WRLCK);
}

static BOOL flock_unlock(int fd) {
	return flock_creat(fd, F_UNLCK);
}

static void read_message(message_t *msg, int fd) {
	char * aux = calloc(MESSAGE_SIZE, sizeof(char));
	int head_poninter;
	sigset_t sigset;
	sigemptyset(&sigset); //TODO: ver como evitar que le lleguen las señales
	sigaddset(&sigset, SIGUSR1);
	int sig;
	header_t *header = read_header(fd);
	/* calculate position from header */
	lseek(fd, MESSAGE_SIZE * header -> head + sizeof(header_t), SEEK_SET); //ver de que el header -> head arranque en 1 y después como manejo el modulo para que no de nunca cero
	while((read(fd, aux, MESSAGE_SIZE) == ERROR) || (header -> head == header -> tail)) {
		flock_unlock(fd);
		sigwait(&sigset, &sig); 
		while(!flock_write(fd))
			;
		header = read_header(fd);
		lseek(fd, MESSAGE_SIZE * header -> head + sizeof(header_t), SEEK_SET); //ver de que el header -> head arranque en 1 y después como manejo el modulo para que no de nunca cero
	}

	msg->sender = ((uint16_t *)aux)[0];
	msg->content_len = ((uint16_t *)aux)[1];
	msg = realloc(msg, sizeof(uint16_t)*2 + msg->content_len);
	memcpy(msg -> content, aux + sizeof(uint16_t)*2, msg->content_len);
	free(aux);
	/* update the header */
	head_poninter = header -> head;
	header -> head = (++ head_poninter) % SIZE_FILE;
	write_header(header, fd);
	free(header);
	flock_unlock(fd);
	return;
}

static void write_message(message_t *msg, int fd) {
	//TODO: falta checkear que tenga lugar para escribir
	int tail_pointer;
	header_t *header = read_header(fd);
	while((header -> tail + 1 == header -> head) || ((header -> tail + 1 == SIZE_FILE) && (header -> head == 0))) {
		sigset_t sigset;
		sigemptyset(&sigset); //TODO: ver como evitar que le lleguen las señales
		sigaddset(&sigset, SIGUSR1);
		int sig;
		flock_unlock(fd);
		sigwait(&sigset, &sig); //TODO: ver bien que es sigset
		while(!flock_write(fd))
			;
		header = read_header(fd);
	}
	/* calculate position from header */
	lseek(fd, MESSAGE_SIZE * header -> tail + sizeof(header_t), SEEK_SET); //TODO: verificar que no haya que hacer alguna modificación extra.
	write(fd, msg, MESSAGE_SIZE);

	tail_pointer = header -> tail;
	header -> tail = (++tail_pointer) % SIZE_FILE;
	write_header(header, fd);
	free(header);
	flock_unlock(fd);
	return;
}

static void write_header(header_t *header, int fd) {
	lseek(fd, 0, SEEK_SET);
	write(fd, header, sizeof(header_t));
}

static header_t *read_header(int fd) {
	char *str = calloc(MESSAGE_SIZE, sizeof(char));
	header_t *header = (header_t*)malloc(sizeof(header_t));
	lseek(fd, 0, SEEK_SET);
	if(read(fd, str, sizeof(header_t)) <= 0) {
		header -> head = 0;
		header -> tail = 0;
	} else {
		memcpy(header, str, sizeof(header_t));
		free(str);
	}
	return header;
}
