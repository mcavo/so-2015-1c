#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "../../inc/ipc/mqueues.h"


ipc_t *ipc_connect(int pid){

	int msqid;
    ipc_t* msq = malloc(sizeof(ipc_t));

    if ((msqid = msgget(pid, 0666)) == -1) { /* connect to the queue */
        perror("msgget");
		fprintf(stderr, "Ocurrio el error %s en ipc_connect\n",strerror(errno));		
	return NULL;
    }
	
    msq->id = msqid;

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

    msq->id = msqid;

    return msq;
}

void ipc_close(ipc_t *ipc){

    if (msgctl(ipc->id, IPC_RMID, NULL) == -1) {
        perror("msgctl");
		fprintf(stderr, "Ocurrio el error %s en ipc_close\n",strerror(errno));		
    }
    free(ipc);
}


void ipc_send(ipc_t *ipc, message_t *message, int s){
    
	buf_t buf;
	int size = sizeof(uint16_t)+*((uint32_t*)(message->content+sizeof(uint8_t)));
	buf.mtype=0;
	memcpy(&(buf.mtext), message, size);
	printf("ipc_id en ipc_send: %d\n",ipc->id);

	if (msgsnd(ipc->id, &buf, size, 0) == -1)
    {
		perror("msgsnd");
		fprintf(stderr, "Ocurrio el error %s en ipc_send\n",strerror(errno));		
		exit(1);
	}

		printf("hizo el send en ipc_send\n\n");	   
	
}

message_t* ipc_receive(ipc_t *ipc){

		char buf[MSG_SIZE];
		int nbytes;

	    if ((nbytes = msgrcv(ipc->id, &buf, MSG_SIZE, 0, 0)) == -1) {
            perror("msgrcv");
			fprintf(stderr, "Ocurrio el error %s en ipc_receive\n",strerror(errno));				
            exit(1);
        }
		
		message_t * msg = malloc(nbytes);
		memcpy(msg, buf + sizeof(long), nbytes);

        return msg;
}
