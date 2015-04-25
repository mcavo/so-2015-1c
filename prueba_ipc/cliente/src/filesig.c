#include "../inc/filesig.h"

static BOOL flock_creat(FILE *fd, int type);
static BOOL flock_write(FILE *fd);
static BOOL flock_unlock(FILE *fd);

static FILE *open_file(ipc_t *ipc);
static void close_file(FILE *fd);
static void read_message(message_t *msg, FILE *fd);
static void write_message(message_t *msg, FILE *fd);
static void itoa ( int value, char * str );
static void write_line(char *msg, FILE *fd);
static header_t *read_header(FILE *fd);
static void parse_header(char *str, header_t *header);
static void reverse(char s[], int size);
static char *get_fname(ipc_t *ipc);
static void write_header(header_t *header, FILE *fd);

static void signal_handler(int signum) {
	printf("%s\n", "wake up"); //TODO: only testing
}

static void install_signal_handler() {
	printf("instalando señal \n");
    struct sigaction handler = {
        .sa_flags   = 0,
        .sa_handler = signal_handler
    };

    sigaction(SIGUSR1, &handler, NULL);
}

ipc_t *ipc_create(pid_t destination_process) {
	char c;
    ipc_t *ipc = (ipc_t*) malloc(sizeof(ipc_t));
    ipc -> to = destination_process;
    ipc -> from = getpid();

    printf("creando ipc from:%d to:%d\n", getpid(), destination_process);
    install_signal_handler();
    //debug
    scanf("%c", &c);

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
	printf("cearrando ipc\n");
}

/* send the message to the ipc destination */
void ipc_send(message_t *msg, ipc_t *ipc) {
	/* charge in fd the open file */
	printf("sending message\n");
	
	FILE *fd = open_file(ipc);

	write_message(msg, fd);

	close_file(fd);

	/* send signal to weak up de destination process */
	kill(ipc -> to, SIGUSR1);
} 

/* read and lock the file craeted with the ipc information */
message_t *ipc_read(ipc_t *ipc) {
	message_t *msg = (message_t*) malloc(sizeof(message_t));
	printf("reading message\n");
	FILE *fd = open_file(ipc);

	read_message(msg, fd); 

	close_file(fd);

	return msg;
}

static FILE *open_file(ipc_t *ipc) {
	printf("open file\n");
	char* fname = get_fname(ipc);
	FILE *fd = fopen(fname, "a+"); /* a+ append at the end of the file */
	if (fd == NULL) {
		printf("hubo un error\n");
		return NULL; //TODO: cómo manejar los errores en ipc??
	}
	while(!flock_write(fd))
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
	printf("intentando bloquear el archivo\n");
    return flock_creat(fd, F_WRLCK);
}

static BOOL flock_unlock(FILE *fd) {
    return flock_creat(fd, F_UNLCK);
}

static void read_message(message_t *msg, FILE *fd) {
	printf("reading...\n");
	char * aux;
	int head_poninter;
	sigset_t sigset;
	sigemptyset(&sigset);
	sigaddset(&sigset, SIGUSR1);
	int sig;
	if(fd == NULL)
		printf("el file descriptor es null\n");
	printf("el file descriptor NO es null\n");
	header_t *header = read_header(fd);

	/* calculate position from header */
	fseek(fd, MESSAGE_SIZE * header -> head, SEEK_CUR); //TODO: verificar que no haya que hacer alguna modificación extra.
	aux = malloc(sizeof(char)*LINE_LENGTH);
	if(fgets(aux, LINE_LENGTH, fd) == NULL) { //TODO: si fuese null que debería pasar.
		/* if there is nothing to read, wait for a signal */
		printf("en espera...\n");
		flock_unlock(fd);
		sigwait(&sigset, &sig); //TODO: ver bien que es sigset
		while(!flock_write(fd))
			;
	} 
	msg->length = atoi(aux);
	printf("message length %d\n", msg->length);
	free(aux);

	fgets(msg -> content, msg->length, fd); //TODO: si fuese null que debería pasar.

	aux = malloc(sizeof(char)*10);
	fgets(aux, LINE_LENGTH, fd); //TODO: si fuese null que debería pasar.	
	msg -> from = atoi(aux);
	printf("message author: %d\n", msg->from);
	free(aux);

	//TODO: cómo modifico el header para que tenga sentido??
	/* update the header file */
	head_poninter = header -> head;
	header -> head = (head_poninter ++) % SIZE_FILE;
	header -> write = TRUE;
	if(header -> tail == header -> head) {
		/* there is nothing to read. */ 
		//header -> read = FALSE;
	} 
	write_header(header, fd);
	free(header);
	return;
}

static void write_message(message_t *msg, FILE *fd) {
	printf("estoy escribiendo\n");
	char *aux = calloc(10, sizeof(char));
	int tail_pointer;
	header_t *header = read_header(fd);

	/* calculate position from header */
	fseek(fd, MESSAGE_SIZE * header -> tail, SEEK_CUR); //TODO: verificar que no haya que hacer alguna modificación extra.

	itoa(msg -> length, aux);
	write_line((char*)strdup(aux), fd);
	free(aux);
	
	write_line(strdup(msg -> content), fd);
	
	aux = calloc(LINE_LENGTH, sizeof(char));
	itoa(msg -> from, aux);
	write_line(strdup(aux), fd);
	free(aux);
	//TODO: cómo modifico el header para que tenga sentido??
	/* update header */
	tail_pointer = header -> tail;
	header -> tail = (tail_pointer ++) % SIZE_FILE;
	header -> read = TRUE;
	if(header -> tail == header -> head) {
		/* there is no space to write */
		//header -> write = FALSE;
	} 
	write_header(header, fd);
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

static void write_line(char *msg, FILE *fd) {
	printf("write line\n");
	//int n = sizeof(msg) / sizeof(msg[0]);
	int n = strlen(msg);
	char *str = calloc(LINE_LENGTH, sizeof(char));
	strcat(str, msg);
	printf("%s\n", str);
	//n = n + 3;
	for(; n < LINE_LENGTH - 2; n++) {
		str[n] = '-';
	}
	str[n] = '\n';
	fputs(str, fd);
	printf("%s\n", str);
	return;
}

static header_t *read_header(FILE *fd) {
	printf("reading header\n");
	char *str = malloc(sizeof(char*)*LINE_LENGTH);
	header_t *header = (header_t*)malloc(sizeof(header_t));
	printf("posicionando cursor\n");
	fseek(fd, 1, SEEK_SET);
	printf("intento leer header\n");
	if(	fgets(str, LINE_LENGTH, fd) == NULL) {
		printf("archivo vacío\n");
		header -> head = 0;
		header -> tail = 0;
		header -> read = TRUE;
		header -> write = TRUE;
		printf("cargue el header\n");
		write_line("0 0 1 1", fd); /* positions to read and write. The last are in TRUE*/
	} else {
		printf("intento entrar al parser\n");
		parse_header(str, header);
	}
	return header;
}

static void parse_header(char *str, header_t *header) {
	printf("Entre al header\n");
	int i = 0;
	int j = 0;
	int k = 0;
	char data[4][MAX_MESSAGES];
	for (; (str[i] != 0) && (str[i] == '\n'); ++i) {
		printf("estoy en el for %d\n", i);
		switch (str[i]) {
			case ' ':
					data[j++][k] = 0;
					k = 0;
					break; 
			default:
					data[j][k++] = str[i];
		}
	}
	header -> head = atoi(data[0]);
	header -> tail = atoi(data[1]);
	header -> read = atoi(data[2]);
	header -> write = atoi(data[3]);
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
    char * p = "../system_files/";
    itoa(ipc -> to, pid);

    path = (char*)malloc((strlen(pid) + 1 + strlen(p))*sizeof(char));
    if(path==NULL)
        return path;
    memcpy(path, p, strlen(p) + 1);

    path = strcat(path, pid);
    path = strcat(path,".txt");
    return path;
}

void write_header(header_t *header, FILE *fd) {
	char *aux = calloc(SIZE_FILE, sizeof(char));
	char *line = calloc(LINE_LENGTH, sizeof(char));
	itoa(header -> head, aux);
	strcat(line, aux);
	free(aux);
	strcat(line, " ");

	aux = calloc(SIZE_FILE, sizeof(char));
	itoa(header -> tail, aux);
	strcat(line, aux);
	free(aux);
	strcat(line, " ");

 	aux = calloc(SIZE_FILE, sizeof(char));
	itoa(header -> read, aux);
	strcat(line, aux);
	strcat(line, " ");
	free(aux);

	aux = calloc(SIZE_FILE, sizeof(char));
	itoa(header -> write, aux);
	strcat(line, aux);
	free(aux);
	write_line(line, fd);
}