#include "../inc/filesig.h"

static BOOL flock_creat(int fd, int type);
static BOOL flock_write(int fd);
static BOOL flock_unlock(int fd);

static int open_file(ipc_t *ipc);
static void close_file(int fd);
static void read_message(message_t *msg, int fd);
static void write_message(message_t *msg, int fd);
static void itoa ( int value, char * str );
static void write_line(char *msg, int fd);
static header_t *read_header(int fd);
static void parse_header(char *str, header_t *header);
static void reverse(char s[], int size);
static char *get_fname(ipc_t *ipc);
static void write_header(header_t *header, int fd);
static int get_string(char *buffer, int length, int fd);
static int put_string(char *str, int fd);

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
	printf("sending message\n");
	
	int fd = open_file(ipc);

	write_message(msg, fd);

	close_file(fd);

	/* send signal to weak up de destination process */
	printf("Voy a mandar la señal\n");
	kill(ipc -> to, SIGUSR1);
} 

/* read and lock the file craeted with the ipc information */
message_t *ipc_read(ipc_t *ipc) {
	message_t *msg = (message_t*) malloc(sizeof(message_t));
	printf("reading message\n");
	int fd = open_file(ipc);
	//printf("%d\n", fd);
	read_message(msg, fd); 

	close_file(fd);

	return msg;
}

static int open_file(ipc_t *ipc) {
	printf("open file\n");
	char* fname = get_fname(ipc);
	int fd = open(fname, O_RDWR | O_CREAT, 0666); 
	if (fd == ERROR) {
		printf("hubo un error\n");
		return ERROR; //TODO: cómo manejar los errores en ipc??
	}
	while(!flock_write(fd))
		;

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
    return flock_creat(fd, F_UNLCK);
}

static void read_message(message_t *msg, int fd) {
	printf("reading...\n");
	char * aux;
	int head_poninter;
	sigset_t sigset;
	sigemptyset(&sigset); //TODO: ver como evitar que le lleguen las señales
	sigaddset(&sigset, SIGUSR1);
	int sig;
	if(fd == ERROR)
		printf("el file descriptor es null\n");
	printf("el file descriptor NO es null\n");
	header_t *header = read_header(fd);

	/* calculate position from header */
	//TODO: verificar que no haya que hacer alguna modificación extra.
	lseek(fd, MESSAGE_SIZE * header -> head, SEEK_CUR); //ver de que el header -> head arranque en 1 y después como manejo el modulo para que no de nunca cero
	aux = malloc(sizeof(char)*LINE_LENGTH);
	while((get_string(aux, LINE_LENGTH, fd) == ERROR) || (header -> read == FALSE)) { //TODO: si fuese null que debería pasar.
		/* if there is nothing to read, wait for a signal */
		printf("en espera...\n");
		//TODO:no es completamente seguro
		flock_unlock(fd);
		sigwait(&sigset, &sig); //TODO: ver bien que es sigset
		printf("desperté!!\n");
		scanf("%c",aux);
		while(!flock_write(fd))
			;
		printf("bloquee de nuevo el archivo\n");
	} 
	msg->length = atoi(aux);
	printf("message length %d\n", msg->length);
	free(aux);

	msg -> content = calloc(msg->length, sizeof(char));
	get_string(msg -> content, msg->length, fd); //TODO: si fuese null que debería pasar.
	printf("%s\n",msg->content);
	lseek(fd, LINE_LENGTH - msg->length, SEEK_CUR); /* consume the rest of the line */

	aux = calloc(LINE_LENGTH, sizeof(char));
	get_string(aux, LINE_LENGTH, fd); //TODO: si fuese null que debería pasar.	
	msg -> from = atoi(aux);
	printf("message author: %d\n", msg->from);
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
	return;
}

static void write_message(message_t *msg, int fd) {
	//TODO: falta checkear que tenga lugar para escribir
	char *aux = calloc(10, sizeof(char));
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
	}
	/* calculate position from header */
	//lseek(fd, MESSAGE_SIZE * header -> tail, SEEK_CUR); //TODO: verificar que no haya que hacer alguna modificación extra.
	lseek(fd, MESSAGE_SIZE * header -> tail, SEEK_CUR);
	itoa(msg -> length, aux);
	write_line((char*)strdup(aux), fd);
	free(aux);
	
	write_line(strdup(msg -> content), fd);
	
	aux = calloc(10, sizeof(char));
	itoa(msg -> from, aux);
	write_line(strdup(aux), fd);
	free(aux);
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

static void write_line(char *msg, int fd) {
	printf("write line\n");
	int n = strlen(msg);
	char *str = calloc(LINE_LENGTH + 1 , sizeof(char));
	strcat(str, msg);
	printf("%s\n", str);
	for(; n < LINE_LENGTH - 1; n++) {
		str[n] = '-';
	}
	str[n] = '\n';
	printf("line string:%s", str);
	put_string(str, fd);
	return;
}

static header_t *read_header(int fd) {
	printf("reading header\n");
	char *str = calloc(LINE_LENGTH, sizeof(char));
	header_t *header = (header_t*)malloc(sizeof(header_t));
	printf("posicionando cursor\n");
	lseek(fd, 0, SEEK_SET);
	printf("intento leer header\n");
	if(	get_string(str, LINE_LENGTH, fd) == ERROR) {
		printf("archivo vacío\n");
		header -> head = 0;
		header -> tail = 0;
		header -> read = TRUE;
		header -> write = TRUE;
		printf("cargue el header\n");
		write_line("0 0 1 1", fd); /* positions to read and write. The last are in TRUE*/
	} else {
		printf("%s\n", str);
		parse_header(str, header);
	}
	return header;
}

static void parse_header(char *str, header_t *header) {
	printf("Intneto parsear header\n");
	printf("%s\n",str );
	int state = START;
	int i = 0;
	int j = 0;
	int k = 0;
	char data[5][MAX_MESSAGES];

	for (; (str[i] != 0) && (state != END) ; ++i) {
		printf("%i - %c\n", i, str[i]);
		switch (str[i]) {
			case '\n':
					state = END;
					break;
			case '-':
			case ' ':
					if(state == SPACE) {
						state = END;
						break;
					}
					state = SPACE;
					data[j++][k] = 0;
					k = 0;
					break; 
			default:
					data[j][k++] = str[i];
					state = START;
		}
	}
	header -> head = atoi(data[0]);
	printf("%s\n", data[0]);
	header -> tail = atoi(data[1]);
	printf("%s\n", data[1]);
	header -> read = atoi(data[2]);
	printf("%s\n", data[2]);
	header -> write = atoi(data[3]);
	printf("%s\n", data[3]);}

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

void write_header(header_t *header, int fd) {
	lseek(fd, 0, SEEK_SET);

	char *aux = calloc(SIZE_FILE, sizeof(char));
	char *line = calloc(LINE_LENGTH, sizeof(char));
	itoa(header -> head, aux);
	printf("%s\n", aux);
	strcat(line, aux);
	free(aux);
	strcat(line, " ");

	aux = calloc(SIZE_FILE, sizeof(char));
	itoa(header -> tail, aux);
	printf("%s\n", aux);
	strcat(line, aux);
	free(aux);
	strcat(line, " ");

 	aux = calloc(SIZE_FILE, sizeof(char));
	itoa(header -> read, aux);
	printf("%s\n", aux);
	strcat(line, aux);
	strcat(line, " ");
	free(aux);

	aux = calloc(SIZE_FILE, sizeof(char));
	itoa(header -> write, aux);
	printf("%s\n", aux);
	strcat(line, aux);
	free(aux);
	write_line(line, fd);
}

static int get_string(char *str, int length, int fd) {
	char *buffer = calloc(length + 10, sizeof(char));
	int state = START;
	int i = 0;
	int ans = read(fd, buffer, (length)* sizeof(char));
	printf("buffer: %s\n", buffer);
	printf("reads ans:%d\n", ans);
	if (ans <= 0) {
		return -1;
	} 
	//TODO: hay que garantizar que la información del método no tenga ni espacios ni \n
	while((buffer[i] != 0) && (state != END)){
		switch (buffer[i]) {
			case '\n':
					if(state == SPACE) {
						buffer[i - 2] = 0;
					}
					state = END;
					break;
			case '-':
			case ' ':
					if(state == SPACE) {
						state = END;
						buffer[i - 2] = 0;
						break;
					}
					state = SPACE;
					str[i] = buffer[i];
					break; 
			default:
					str[i]=buffer[i];
					state = START;
		}
		i++;
	}
	buffer[i] = 0;
	printf("%s\n", str);
	return ans;
}

static int put_string(char *str, int fd) {
	int ans = write(fd, str, strlen(str)*sizeof(char));
	printf("puts str: %s", str);
	printf("puts ans:%d\n", ans);
	if(ans <= 0){
		return -1;
	}
	return ans;
}

