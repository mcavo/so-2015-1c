#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "../../inc/ipc/mqueues.h"


ipc_t *ipc_connect(int pid){

	int msqid;
    ipc_t* msq = malloc(sizeof(ipc_t));

    if ((msqid = msgget(pid, 0666)) == -1) { /* connect to the queue */
        perror("msgget");
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
	return NULL;
    }

    msq->id = msqid;

    return msq;
}

void ipc_close(ipc_t *ipc){

    if (msgctl(ipc->id, IPC_RMID, NULL) == -1) {
        perror("msgctl");
    }
    free(ipc);
}


void ipc_send(ipc_t *ipc, message_t *message, int size){
    
    if (msgsnd(ipc->id, message, size, 0) == -1)
    	perror("msgsnd");
}

message_t* ipc_receive(ipc_t *ipc){

	    message_t * message = malloc(MSG_SIZE);

	    if (msgrcv(ipc->id, message, MSG_SIZE, 0, 0) == -1) {
            perror("msgrcv");
            exit(1);
        }

        return message;

}
