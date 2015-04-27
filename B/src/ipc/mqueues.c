#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include "../../inc/ipc/mqueues.h"


ipc_t *ipc_connect(int pid,int server_id){

    ipc_t* msq = ipc_open(pid); //alloco la estructura y seteo el id de la queue
	
    msq->server_id=server_id;
    msq->id=getpid();

    return msq;
}

//Lo llama el server por ejemplo, para crear un unico canal por donde recibe las peticiones de los clientes.
ipc_t* ipc_open(int pid){

	//por si no compila el msgget con pid: key_t key = (key_t)pid;
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

    if (msgctl(ipc->queue, IPC_RMID, NULL) == -1) {
        perror("msgctl");
		fprintf(stderr, "Ocurrio el error %s en ipc_close\n",strerror(errno));		
    }
    free(ipc);
}


void ipc_send(ipc_t *ipc, uint16_t recipient, void *message, uint16_t len){
    
	printf("size= %d\n", len);
	buf_t* buf=malloc(sizeof(buf)+sizeof(message_t)+len);
	buf->mtype=recipient;
	(buf->mtext).sender = ipc->id;
	(buf->mtext).content_len = len; 
	memcpy((buf->mtext).content, message, len);
	printf("mtext sender en ipc_send: %d\n",(buf->mtext).sender);
	
	printf("ipc_id en ipc_send: %d\n",ipc->id);
	
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
