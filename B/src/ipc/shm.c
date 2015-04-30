#include "../../inc/ipc/shm.h"

static void fatal(char *s);
/* Semaforos */
static int open_mutex(key_t key);
static void init_mutex(int semid);
static int close_mutex(int semid);
static void dec(int semid, int numsem);
static void inc(int semid,int numsem);
/* Shared memory */
static message_t* get_memory(ipc_t *ipc);
static void free_memory(message_t * mem);

ipc_t *ipc_connect(int pid) {
  ipc_t *ipc = ipc_open(pid);
  open_mutex(pid);
  return ipc;
}

ipc_t *ipc_listen(int pid) {
  ipc_t* ipc = ipc_open(pid); 
  int semid = open_mutex(pid);
  init_mutex(semid); // semaphores 0 0
  return ipc;     
}

ipc_t* ipc_open(int pid) {
  ipc_t *ipc = (ipc_t*) malloc(sizeof(ipc_t));
  ipc->server_id = get_server_pid();
  ipc->id = getpid();
  ipc->shmid = pid;
  ipc->shm = get_memory(ipc);
  return ipc;     
}

void ipc_close(ipc_t *ipc) {
  if(ipc->id == ipc->server_id){
    int semid;
    if( (semid = semget(ipc->id, 2, IPC_CREAT | 0666)) == -1 )
      fatal("semget failure");
    close_mutex(semid);
    free_memory(ipc->shm);
  }
  free(ipc);
}

void ipc_send(ipc_t *ipc, uint16_t recipient, void *message, uint16_t len) {
  int semid;
  if((semid = semget(ipc->shmid, 2, 0666)) == -1)
    fatal("semget failure");
  if(ipc->id == ipc->server_id) 
    dec(semid,SERVER_CAN_WRITE);
  else 
    dec(semid,CLIENT_CAN_WRITE);
  memcpy(&(ipc->shm)->sender, &ipc->id, sizeof(ipc->id));
  memcpy(&(ipc->shm)->content_len, &len, sizeof(len));
  memcpy((ipc->shm)->content, message, len);
  if(ipc->id == ipc->server_id) 
    inc(semid,CLIENT_CAN_READ);
  else 
    inc(semid,SERVER_CAN_READ);
}

message_t* ipc_receive(ipc_t *ipc) {
  uint16_t sender;
  uint16_t len;
  int semid;
  if((semid = semget(ipc->shmid, 2, 0666)) == -1)
    fatal("semget failure");
  if(ipc->id == ipc->server_id) 
    dec(semid,SERVER_CAN_READ);
  else
    dec(semid,CLIENT_CAN_READ);

  memcpy(&sender, &(ipc->shm)->sender, sizeof(sender));
  memcpy(&len, &(ipc->shm)->content_len, sizeof(len)); 

  size_t message_size = sizeof(message_t) + len;
  message_t* msg = (message_t*) malloc(message_size);
  memcpy(msg->content, (ipc->shm)->content, len);

  msg->sender = sender;
  msg->content_len = len;
  
  if(ipc->id == ipc->server_id) 
    inc(semid,SERVER_CAN_WRITE);
  else
    inc(semid,CLIENT_CAN_WRITE);
    
  return msg;
}

static void fatal(char *s) {
  perror(s);
  exit(1);
}

static int open_mutex(key_t key) {
  int aux;
  if( (aux = semget(key, 2, IPC_CREAT | 0666)) == -1 )
    fatal("semget failure");
  return aux;
}

static void init_mutex(int semid) {
  semctl(semid, 0, SETVAL, 1);
  semctl(semid, 1, SETVAL, 0);
}

static int close_mutex(int semid){
  int exit_status = EXIT_SUCCESS;
  exit_status = (semctl(semid, 0, IPC_RMID) != 0);
  return exit_status;
}
 
static void dec(int semid, int numsem) {
  struct sembuf sb;
  sb.sem_num = numsem - 1;
  sb.sem_op = -1;
  sb.sem_flg = 0;
  if( semop(semid,&sb,1) == -1 ) 
    fatal("semop failure");
}

static void inc(int semid,int numsem) {
  struct sembuf sb; //El orden de los elementos puede variar, por eso se define asi
  sb.sem_num = numsem - 1;
  sb.sem_op = 1;
  sb.sem_flg = 0;
  if( semop(semid,&sb,1) == -1 ) 
    fatal("semop failure");
}

static message_t* get_memory(ipc_t *ipc) {
  message_t * data;
  int shmid;
  int shmsz = sizeof(message_t);
  if ((shmid = shmget(ipc->shmid, shmsz, 0666 | IPC_CREAT)) == -1) {
    perror("shmget");
    exit(1);
  }
  /* attach to the segment to get a pointer to it: */
  data = shmat(shmid, NULL, 0);
  if (data == (message_t *)(-1)) {
    perror("shmat");
    exit(1);
  }
  return data;
}

static void free_memory(message_t * data) {
  int exit_status;
  exit_status = shmdt(data);
  if(exit_status == -1) 
    fatal("shm_unlink failure");
}