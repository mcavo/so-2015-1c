#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "../../inc/ipc/mqueues.h"




//de la estructura ipc_t SOLO ESTOY USANDO EL ID que sería lo que identifica a la message queue.
//No se para que usar estas dos aca!!
//ipc_t *ipc_listen(char *address);

/*El connect lo voy a usar para los que tengan que abrir un ipc YA CREADO. Como por ejemplo, los clientes para 
	enviarle comandos al server. Y el worker para abrir el ipc que el cliente creo para que le respondan ahi.
	El ftok va a tener que usar LOS MISMOS PARAMETROS QUE CUANDO FUE CREADO EL IPC para poder conectarse con el.
*/
ipc_t *ipc_connect(char *address){

	int msqid;
    key_t key;
    if ((key = ftok(address, 'A')) == -1) {  /* Misma key que se uso en el ipc_open. TIENE QUE COINCIDIR EL address y el 'A'*/
        perror("ftok");
        exit(1);
    }
    if ((msqid = msgget(key, 0666)) == -1) { /* connect to the queue */
        perror("msgget");
        exit(1);
    }

}

//Lo llama el server por ejemplo, para crear un unico canal por donde recibe las peticiones de los clientes.
ipc_t* ipc_open(char *root){

	key_t key;
	int msqid;
    ipc_t msq;

    //ftok("mqueues.c", 'A')
    if ((key = ftok(root, 'A')) == -1) {
        perror("ftok");
        exit(1);
    }
    if ((msqid = msgget(key, 0666 | IPC_CREAT)) == -1) {
        perror("msgget");
        exit(1);
    }

    msq.id = msqid;

    return &msq;
}

void ipc_close(ipc_t *ipc){

    if (msgctl(ipc->id, IPC_RMID, NULL) == -1) {
        perror("msgctl");
        exit(1);
    }
}

//No estoy usando recipient
void ipc_send(ipc_t *ipc, uint16_t recipient, void *message, uint16_t size){
    
    if (msgsnd(ipc->id, &message, size, 0) == -1)
    	perror("msgsnd");
}

message_t* ipc_receive(ipc_t *ipc){

		message_t message;

	    if (msgrcv(msqid, &message, sizeof(buf.mtext), 0, 0) == -1) {
            perror("msgrcv");
            exit(1);
        }

        return &message;

}
