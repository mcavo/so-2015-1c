#include "../../inc/ipc/shmposix.h"

static int id, server_id;
static char *addr;

static void init_mutex(void);
static int close_mutex(void);

static message_t* get_memory(char* root, ipc_t *ipc);
static void free_memory(char *addr);

static void handle_signal(int signal);
static void install_signal_handler();

static void dec(int numsem);
static void inc(int numsem);

static sem_t *sem1;
static sem_t *sem2;
static sem_t *sem3;
static sem_t *sem4;

ipc_t* ipc_listen(char *address){
	
	ipc_t *ipc = (ipc_t*) malloc(sizeof(ipc_t));
	ipc->server_id = ipc->id = getpid();
	id = ipc->id;
	server_id = ipc->server_id;
	ipc->addr = address + 2; // /addr used in shm_unlink function
	addr = ipc->addr;
	ipc->shared_memory = get_memory(address + 2, ipc);
	install_signal_handler();
	init_mutex(); // semaphores 0 0 0 0
	inc(CLIENT_WRITE);
	return ipc;
}
ipc_t *
ipc_connect(char *file) {
ipc_t *ipc = (ipc_t*) malloc(sizeof(ipc_t));
ipc->id = getpid();
ipc->server_id = atoi(filename(file));
id = ipc->id;
server_id = ipc->server_id;
char *f = filepath(file);
ipc->addr = f + 1;
ipc->shared_memory = get_memory(f + 2, ipc);
install_signal_handler();
init_mutex();
return ipc;
}
void
ipc_close(ipc_t *ipc){
if(ipc->id == ipc->server_id){ // is server?
close_mutex();
free_memory(ipc->addr);
}
free(ipc);
}
void
install_signal_handler(){
struct sigaction sa;
// Setup the sighub handler
sa.sa_handler = &handle_signal;
// Restart the system call, if at all possible
sa.sa_flags = SA_RESTART;
// Block every signal during the handler
sigfillset(&sa.sa_mask);
if (sigaction(SIGINT, &sa, NULL) == -1) {
fatal("Error: cannot handle SIGINT");
}
}
void
handle_signal(int signal){
if(id == server_id){
close_mutex();
free_memory(addr);
}
exit(0);
}
void
ipc_send(ipc_t *ipc, uint16_t recipient, void *message, uint16_t length)
{
if(ipc->id == ipc->server_id) dec(SERVER_WRITE);
if(ipc->id != ipc->server_id) dec(CLIENT_WRITE);
memcpy(&(ipc->shared_memory)->sender, &ipc->id, sizeof(ipc->id));
memcpy(&(ipc->shared_memory)->content_length, &length, sizeof(length));
memcpy((ipc->shared_memory)->content, message, length);
sleep(1);
if(ipc->id == ipc->server_id) inc(CLIENT_READ);
if(ipc->id != ipc->server_id) inc(SERVER_READ);
}
message_t*
ipc_recv(ipc_t *ipc)
{
uint16_t sender;
uint16_t length;
if(ipc->id == ipc->server_id) dec(SERVER_READ);
if(ipc->id != ipc->server_id) dec(CLIENT_READ);
memcpy(&sender, &(ipc->shared_memory)->sender, sizeof(sender));
memcpy(&length, &(ipc->shared_memory)->content_length, sizeof(length));
/* Alloc and read content: */
size_t message_size = sizeof(message_t) + length;
message_t* msg = (message_t*) malloc(message_size);
memcpy(msg->content, (ipc->shared_memory)->content, length);
msg->sender = sender;
msg->content_length = length;
sleep(1);
if(ipc->id == ipc->server_id) inc(SERVER_WRITE);
if(ipc->id != ipc->server_id) inc(CLIENT_WRITE);
return msg;
}

/*******************************************************/
/********************SHARED MEMORY**********************/
/*******************************************************/

message_t* get_memory(char* root, ipc_t *ipc){
	int fd, ft, shmsz;
	message_t *mem;
	shmsz = sizeof(message_t);
	fd = shm_open(root, O_RDWR | O_CREAT, 0666);
	ft = ftruncate(fd, shmsz);
	if( ft == -1) 
		fatal("ftruncate failure");
	mem = mmap(NULL, shmsz, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if(mem == MAP_FAILED) 
		fatal("mmap failure");
	if(close(fd)) 
		fatal("close failure");
	return mem;
}

void free_memory(char *addr) {
	int exit_status;
	exit_status = shm_unlink(addr);
	if(exit_status == -1) 
		fatal("shm_unlink failure");
}

void
init_mutex(void)
{
	if ( !(sem1 = sem_open("/sem1", O_RDWR | O_CREAT, 0666, 0)) )
		fatal("sem_open1 failure");
	if ( !(sem2 = sem_open("/sem2",O_RDWR | O_CREAT, 0666, 0)) )
		fatal("sem_open2 failure");
	if ( !(sem3 = sem_open("/sem3",O_RDWR | O_CREAT, 0666, 0)) )
		fatal("sem_open3 failure");
	if ( !(sem4 = sem_open("/sem4",O_RDWR | O_CREAT, 0666, 0)) )
		fatal("sem_open4 failure");
}

static int close_mutex(void){
	int exit_status = EXIT_SUCCESS;
	if ( sem_close(sem1) != 0 ) 
		exit_status = EXIT_FAILURE;
	if ( sem_unlink("/sem1") != 0) 
		exit_status = EXIT_FAILURE;
	if ( sem_close(sem2) != 0 ) 
		exit_status = EXIT_FAILURE;
	if ( sem_unlink("/sem2") != 0) 
		exit_status = EXIT_FAILURE;
	if ( sem_close(sem3) != 0 ) 
		exit_status = EXIT_FAILURE;
	if ( sem_unlink("/sem3") != 0) 
		exit_status = EXIT_FAILURE;
	if ( sem_close(sem4) != 0 ) 
		exit_status = EXIT_FAILURE;
	if ( sem_unlink("/sem4") != 0) 
		exit_status = EXIT_FAILURE;
	return exit_status;
}

static void dec(int numsem) {
	switch(numsem){
		case CLIENT_CAN_WRITE: 
			sem_wait(sem1); 
			break;
		case SERVER_CAN_READ : 
			sem_wait(sem2); 
			break;
		case CLIENT_CAN_READ : 
			sem_wait(sem3); 
			break;
		case SERVER_CAN_WRITE : 
			sem_wait(sem4); 
			break;
	}
}

static void inc(int numsem) {
	switch(numsem){
		case CLIENT_CAN_WRITE: 
			sem_post(sem1); 
			break;
		case SERVER_CAN_READ : 
			sem_post(sem2); 
			break;
		case CLIENT_CAN_READ : 
			sem_post(sem3); 
			break;
		case SERVER_CAN_WRITE : 
			sem_post(sem4); 
			break;
	}
}
