#include "../../inc/ipc/mqueues.h"
#include "../../inc/utils.h"

ipc_t *ipc_listen(int pid) {
	ipc_t * msq = ipc_open(pid);
	msq->server_id=get_server_pid();
	msq->id=getpid();
	return msq;
}

<<<<<<< HEAD
ipc_t *ipc_connect(int pid){
	int msqid;
	ipc_t* msq = malloc(sizeof(ipc_t));
	if ((msqid = msgget(pid, 0666)) == -1) {
		perror("msgget");
		fprintf(stderr, "Ocurrio el error %s en ipc_open\n",strerror(errno));		
		return NULL;
  }
  msq->queue = msqid;
	msq->server_id=get_server_pid();
	msq->id=getpid();
	return msq;
}

ipc_t* ipc_listen(int pid){
	return ipc_connect(pid);
}

//Lo llama el server por ejemplo, para crear un unico canal por donde recibe las peticiones de los clientes.
ipc_t* ipc_open(int pid){
	int msqid;
	ipc_t* msq = malloc(sizeof(ipc_t));
	if ((msqid = msgget(pid, 0666 | IPC_CREAT)) == -1) {
		perror("msgget");
		fprintf(stderr, "Ocurrio el error %s en ipc_open\n",strerror(errno));		
		return NULL;
  }
  msq->queue = msqid;
  return msq;
}

void ipc_close(ipc_t *ipc){
	if(ipc->id==ipc->server_id) {
	    if (msgctl(ipc->queue, IPC_RMID, NULL) == -1) {
	        perror("msgctl");
			fprintf(stderr, "Ocurrio el error %s en ipc_close\n",strerror(errno));
		}	
    }
    free(ipc);
}


void ipc_send(ipc_t *ipc, uint16_t recipient, void *message, uint16_t len){
    
	buf_t* buf=malloc(sizeof(buf)+sizeof(message_t)+len);
	buf->mtype=recipient;
	(buf->mtext).sender = ipc->id;
	(buf->mtext).content_len = len; 
	memcpy((buf->mtext).content, message, len);
	
	if (msgsnd(ipc->queue, buf, sizeof(*buf) - sizeof(long) + len, 0) == -1)
    {
		perror("msgsnd");
		fprintf(stderr, "Ocurrio el error %s en ipc_send\n",strerror(errno));		
		exit(1);
	}
	free(buf);

}

message_t* ipc_receive(ipc_t *ipc){

	static char buf[MSG_SIZE];
	int nbytes;

	    if ((nbytes = msgrcv(ipc->queue, &buf, MSG_SIZE, ipc->id, 0)) == -1) {
            perror("msgrcv");
			fprintf(stderr, "Ocurrio el error %s en ipc_receive\n",strerror(errno));				
            exit(1);
        }
		
		message_t * msg = malloc(nbytes);
		memcpy(msg, buf + sizeof(long), nbytes);

        return msg;
}
