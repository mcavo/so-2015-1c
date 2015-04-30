
#include "../../inc/ipc/filesig.h"

static BOOL flock_creat(int fd, int type);
static BOOL flock_write(int fd);
static BOOL flock_unlock(int fd);
static int open_file(ipc_t *ipc);
static void close_file(int fd);
static void read_message(message_t *msg, int fd);
static void write_message(message_t *msg, int fd);
static header_t *read_header(int fd);
// static void parse_header(char *str, header_t *header);
static void write_header(header_t *header, int fd);
// static int get_string(char *buffer, int length, int fd);
// static int put_string(char *str, int fd);

static void signal_handler(int signum) {
	printf("%s\n", "wake up"); //TODO: only testing
}

static void install_signal_handler() {
	printf("instalando señal \n");
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
	printf("creando ipc from:%d to:%d\n", getpid(), destination_process);
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
	//char *path = get_fname(ipc -> server_id);
	close_file(ipc -> fd);
	//unlink(path);
	free(ipc);
	printf("cearrando ipc\n");
}
/* send the message to the ipc destination */
void ipc_send(ipc_t *ipc, uint16_t recipient, void *message, uint16_t len) {
	while(!flock_write(ipc -> fd))
		;
	printf("sending message\n");
	message_t *msg = calloc(MESSAGE_SIZE, sizeof(char)); //TODO: el máximo debería ser más chico.
	msg -> sender = getpid();
	printf("client pid:%d\n", msg -> sender);
	msg -> content_len = len;
	memcpy(msg->content, message, len);
	write_message(msg, ipc -> fd);
	/* send signal to weak up de destination process */
	free(msg);
	printf("Voy a mandar la señal a: %d\n", ipc -> server_id);
	sleep(10);
	kill(ipc -> server_id, SIGUSR1);
}
/* read and lock the file craeted with the ipc information */
message_t *ipc_receive(ipc_t *ipc) {
	while(!flock_write(ipc -> fd))
		;
	message_t *msg = (message_t*) calloc(1, sizeof(message_t));
	printf("reading message\n");
	//printf("%d\n", fd);
	read_message(msg, ipc -> fd);
	printf("terminé de leer y retorno\n");
	return msg;
}

static int open_file(ipc_t *ipc) {
	printf("open file\n");
	char* fname = get_fname(ipc -> server_id);
	printf("fname: %s\n", fname);
	int fd = open(fname, O_RDWR | O_CREAT, 0666);
	if (fd == ERROR) {
		printf("hubo un error\n");
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
	printf("intentando bloquear el archivo\n");
	return flock_creat(fd, F_WRLCK);
}

static BOOL flock_unlock(int fd) {
	printf("desbloqueando el archivo\n");
	return flock_creat(fd, F_UNLCK);
}

static void read_message(message_t *msg, int fd) {
	printf("reading...\n");
	char * aux = calloc(MESSAGE_SIZE, sizeof(char));
	int head_poninter;
	sigset_t sigset;
	sigemptyset(&sigset); //TODO: ver como evitar que le lleguen las señales
	sigaddset(&sigset, SIGUSR1);
	int sig;
	header_t *header = read_header(fd);
	printf("header: %d, %d, %d, %d\n",header -> head, header->tail, header -> read, header -> write);
	/* calculate position from header */
	//TODO: verificar que no haya que hacer alguna modificación extra.
	printf("posición del seek: %d\n", MESSAGE_SIZE * header -> head + sizeof(header_t));
	lseek(fd, MESSAGE_SIZE * header -> head + sizeof(header_t), SEEK_SET); //ver de que el header -> head arranque en 1 y después como manejo el modulo para que no de nunca cero
	int i = read(fd, aux, MESSAGE_SIZE);
	while((i == ERROR) || (header -> read == FALSE)) { //TODO: si fuese null que debería pasar.
		printf("header -> read : %d\n", header -> read);
		printf("lectrura del read: %d\n", i);
		/* if there is nothing to read, wait for a signal */
		printf("en espera...\n");
		//TODO:no es completamente seguro
		flock_unlock(fd);
		sigwait(&sigset, &sig); //TODO: ver bien que es sigset
		while(!flock_write(fd))
			;
		printf("bloquee de nuevo el archivo\n");
		header = read_header(fd);
		printf("header: %d, %d, %d, %d\n",header -> head, header->tail, header -> read, header -> write);
		printf("posición del seek: %d\n", MESSAGE_SIZE * header -> head + sizeof(header_t));
		lseek(fd, MESSAGE_SIZE * header -> head + sizeof(header_t), SEEK_SET); //ver de que el header -> head arranque en 1 y después como manejo el modulo para que no de nunca cero
		i = read(fd, aux, MESSAGE_SIZE);
	}

	msg->sender = ((uint16_t *)aux)[0];
	printf("sender id: %d\n", msg -> sender);
	msg->content_len = ((uint16_t *)aux)[1];
	msg = realloc(msg, sizeof(uint16_t)*2 + msg->content_len);
	memcpy(msg -> content, aux + sizeof(uint16_t)*2, msg->content_len);
	free(aux);
	//TODO: cómo modifico el header para que tenga sentido??
	/* update the header */
	head_poninter = header -> head;
	header -> head = (++ head_poninter) % SIZE_FILE;
	printf("el puntero de la cabeza es: %d\n", header->head);
	header -> write = TRUE;
	if(header -> tail == header -> head) {
		/* there is nothing to read. */
		header -> read = FALSE;
	}
	write_header(header, fd);
	free(header);
	flock_unlock(fd);
	return;
}

static void write_message(message_t *msg, int fd) {
	//TODO: falta checkear que tenga lugar para escribir
	int tail_pointer;
	header_t *header = read_header(fd);
	printf("%d\n", header -> write);
	while((header -> write) == FALSE){
		sigset_t sigset;
		sigemptyset(&sigset); //TODO: ver como evitar que le lleguen las señales
		sigaddset(&sigset, SIGUSR1);
		int sig;
		printf("en espera para escribir...\n");
		flock_unlock(fd);
		sigwait(&sigset, &sig); //TODO: ver bien que es sigset
		while(!flock_write(fd))
			;
		header_t *header = read_header(fd);
		printf("header: %d, %d, %d, %d\n",header -> head, header->tail, header -> read, header -> write);
	}
	/* calculate position from header */
	lseek(fd, MESSAGE_SIZE * header -> tail + sizeof(header_t), SEEK_SET); //TODO: verificar que no haya que hacer alguna modificación extra.
	int i = write(fd, msg, MESSAGE_SIZE);
	printf("caracteres escritos: %d\n", i);
	printf("caracteres escritor: %d\n", (uint16_t *)msg);

	//TODO: cómo modifico el header para que tenga sentido??
	/* update header */
	tail_pointer = header -> tail;
	header -> tail = (++tail_pointer) % SIZE_FILE;
	printf("puntero a la cola: %d\n", header->tail);
	header -> read = TRUE;
	if(header -> tail == header -> head) {
	/* there is no space to write */
			header -> write = FALSE;
	}
	write_header(header, fd);
	free(header);
	flock_unlock(fd);
	return;
}

static void write_header(header_t *header, int fd) {
	lseek(fd, 0, SEEK_SET);
	int i = write(fd, header, sizeof(header_t));
	printf("bytes impresos del header%d\n", i);
	printf("header: %d, %d, %d, %d\n",header -> head, header->tail, header -> read, header -> write);
}

static header_t *read_header(int fd) {
	printf("reading header\n");
	char *str = calloc(MESSAGE_SIZE, sizeof(char));
	header_t *header = (header_t*)malloc(sizeof(header_t));
	printf("posicionando cursor\n");
	lseek(fd, 0, SEEK_SET);
	printf("intento leer header\n");
	int i = read(fd, str, sizeof(header_t));
	if( i <= 0) {
		printf("archivo vacío\n");
		header -> head = 0;
		header -> tail = 0;
		header -> read = FALSE;
		header -> write = TRUE;
		printf("cargue el header\n");
		//write_header(header, fd);
	} else {
		printf("header %s\n", str);
		memcpy(header, str, sizeof(header_t));
		free(str);
	}
	printf("bytes leidos del header %d\n", i);
	return header;
}
