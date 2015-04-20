#include "../inc/filesig.h"

static BOOL flock_creat(FILE *fd, int type);
static BOOL flock_write(FILE *fd);
static BOOL flock_unlock(FILE *fd);

static FILE *open_file(ipc_t *ipc);
static void close_file(FILE *fd);
static void read_message(message_t *msg, FILE *fd);
static void write_message(message_t *msg, FILE *fd);
static void itoa ( int value, char * str );
static void write_line(char *str, FILE *fd);
static header_t *read_header(FILE *fd);
static void parse_header(char *str, header_t *header);
static void reverse(char s[], int size);
static char *get_fname(ipc_t *ipc);

static void signal_handler(int signum) {
	printf("%s\n", "wake up"); //TODO: only testing
}

static void install_signal_handler() {
    struct sigaction handler = {
        .sa_flags   = 0,
        .sa_handler = signal_handler
    };

    sigaction(SIGUSR1, &handler, NULL);
}

ipc_t *ipc_create(pid_t destination_process) {
    ipc_t *ipc = (ipc_t*) malloc(sizeof(ipc_t));
    ipc -> to = destination_process;
    ipc -> from = getpid();

    install_signal_handler();

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
	char *path = get_fname(ipc);
	unlink(path);
	free(ipc);
}

/* send the message to the ipc destination */
void ipc_send(message_t *msg, ipc_t *ipc) {
	/* charge in fd the open file */
	FILE *fd = open_file(ipc);

	write_message(msg, fd);

	close_file(fd);

	/* send signal to weak up de destination process */
	kill(ipc -> to, SIGUSR1);
} 

/* read and lock the file craeted with the ipc information */
message_t *ipc_read(ipc_t *ipc) {
	message_t *msg = (message_t*) malloc(sizeof(message_t));

	FILE *fd = open_file(ipc);

	read_message(msg, fd); 

	close_file(fd);

	return msg;
}

static FILE *open_file(ipc_t *ipc) {
	char* fname = get_fname(ipc);
	FILE *fd = fopen(fname, "a+"); /* a+ append at the end of the file */
	if (fd == NULL) {
		return NULL; //TODO: cómo manejar los errores en ipc??
	}
	while(flock_write(fd))
		;

	return fd;
}

static void close_file(FILE *fd) {	
	flock_unlock(fd);
	fclose(fd);
}
//TODO: no hay que liberar el fl??
static BOOL flock_creat(FILE *fd, int type) {
    struct flock fl;

    fl.l_type = type;
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;
    fl.l_len = 0;
    fl.l_pid = getpid();

    if (fcntl(fileno(fd), F_SETLKW, &fl) == -1) {
        return FALSE;
    }
    return TRUE;
}

static BOOL flock_write(FILE *fd) {
    return flock_creat(fd, F_WRLCK);
}

static BOOL flock_unlock(FILE *fd) {
    return flock_creat(fd, F_UNLCK);
}

static void read_message(message_t *msg, FILE *fd) {
	char * aux;
	sigset_t sigset;
	int sig;
	header_t *header = read_header(fd);

	/* calculate position from header */
	fseek(fd, MESSAGE_SIZE * header -> read_count, SEEK_CUR); //TODO: verificar que no haya que hacer alguna modificación extra.
	aux = malloc(sizeof(char)*10);
	if(fgets(aux, LINE_LENGTH, fd) == NULL) { //TODO: si fuese null que debería pasar.
		/* if there is nothing to read, wait for a signal */
		printf("en espera...\n");
		sigwait(&sigset, &sig); //TODO: ver bien que es sigset
	} 
	msg->length = atoi(aux);
	free(aux);

	fgets(msg -> content, msg->length, fd); //TODO: si fuese null que debería pasar.

	aux = malloc(sizeof(char)*10);
	fgets(aux, LINE_LENGTH, fd); //TODO: si fuese null que debería pasar.	
	msg -> from = atoi(aux);
	free(aux);

	//TODO: cómo modifico el header para que tenga sentido??
	free(header);
	return;
}

static void write_message(message_t *msg, FILE *fd) {
	char *aux = calloc(4, sizeof(char));

	header_t *header = read_header(fd);

	/* calculate position from header */
	fseek(fd, MESSAGE_SIZE * header -> write_count, SEEK_CUR); //TODO: verificar que no haya que hacer alguna modificación extra.

	itoa(msg -> length, aux);
	write_line((char*)strdup(aux), fd);
	free(aux);
	
	write_line(strdup(msg -> content), fd);
	
	aux = calloc(10, sizeof(char));
	itoa(msg -> from, aux);
	write_line(strdup(aux), fd);
	free(aux);
	//TODO: cómo modifico el header para que tenga sentido??
	free(header);
	return;
}

/* only for positive ints */
static void itoa ( int value, char * str ) {
	int base = 10;
	int i = 0;	
	do {
		str[i++] = value % base + '0'; 
	} while ((value /= base) > 0);
	
	str[i] = '\0';
	reverse(str, i);
	return;
}

static void write_line(char *str, FILE *fd) {
	int n = sizeof(str) / sizeof(str[0]);
	for(; n < LINE_LENGTH - 1; n++) {
		str[n] = ' ';
	}
	str[n] = '\n';
	fputs(str, fd);
	return;
}

static header_t *read_header(FILE *fd) {
	char *str = malloc(sizeof(char*)*LINE_LENGTH);
	header_t *header = (header_t*)malloc(sizeof(header_t));
	fseek(fd, 1, SEEK_SET);
	if(	fgets(str, LINE_LENGTH, fd) == NULL) {
		header -> read_count = 0;
		header -> write_count = 0;
		header -> modified_order = FALSE;
		write_line("0 0 0", fd);
	} else {
		parse_header(str, header);
	}
	return header;
}

static void parse_header(char *str, header_t *header) {
	int i = 0;
	int j = 0;
	int k = 0;
	char data[3][MAX_MESSAGES];
	for (; (str[i] != 0) || (str[i] == '\n') ; ++i) {
		switch (str[i]) {
			case ' ':
					data[j++][k] = 0;
					k = 0;
					break; 
			default:
					data[j][k++] = str[i];
		}
	}
	header -> read_count = atoi(data[0]);
	header -> write_count = atoi(data[1]);
	header -> modified_order = atoi(data[2]);
}

static void reverse(char s[], int size) {
	int i;
	for(i = 0; i < size/2; i++) {
		char aux = s[i];
		s[i] = s[size - i - 1];
		s[size - i - 1] = aux;
	}
	s[size] = '\0';
}

static char *get_fname(ipc_t *ipc) {
	char *pid = malloc(sizeof(char*)*10);
    char * path;
    char * p = "./file_system/";
    itoa(ipc -> to, pid);

    path = (char*)malloc((strlen(pid) + 1 + strlen(p))*sizeof(char));
    if(path==NULL)
        return path;
    memcpy(path, p, strlen(p) + 1);

    path = strcat(path, pid);
    path = strcat(path,".txt");
    return path;
}

