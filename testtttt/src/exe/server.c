#include "actions.h"
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>


static void worker(message_t * msg);
static uint8_t command_interpreter(message_t msg)

database_t *db;
ipc_t *ipc;

void save_server_pid(){
   FILE * fp;

   fp = fopen ("serverPid.txt", "w+");
   fprintf(fp, "%d", server_pid);
   
   fclose(fp);
}


int main(void)
{
    pid_t pid, server_pid;
    message_t* cmd;
    server_pid = getpid();   
   
    //Dejamos en un archivo el pid del servidor para que los clientes lo tomen y se comuniquen. 
    save_server_pid(server_pid);
 
    //Creamos el ipc unico, cuyo id es el pid del servidor.
    ipc= ipc_open(server_pid); 	

    while(1){

        cmd = ipc_receive(ipc);
        switch(pid = fork()) {
        case -1:
            perror("fork");  /* something went wrong */
            exit(1);         /* parent exits */
        case 0:
            /*El doble fork genera que el nieto sea el worker. El hijo inmediatamente hace exit, haciendo que el padre en su default lo tome en el wait y 
        siga esperando otro comando. El nieto, al morir su padre (hijo) queda colgado del init. Eso genera que el padre no se quede esperando la respuesta, para poder recibir otro comando.
        La respuesta se la manda el worker al cliente directamente y al morir init lo barre. Es decir nadie queda a la espera de la respuesta del worker.
        El padre puede pisar la estructura command porq el hijo al hacer fork tiene su propia copia de todas las variables. */
            switch(fork()){
                case 0:
                    worker(&cmd);//muere solo

                case -1:
                    perror("fork2");
                    exit(1);
                default:
                    exit(0);//El padre del nieto (hijo) termina inmediatamente. En el default del fork de afuera esta esperando esta rta.
            }
        default:
            waitpid(pid,null,0);
        }
    
    }
        return 0;
}

static void worker(message_t * msg) {
    uint8_t command = command_interpreter(msg);
    ipc_t* ipc;
    message_t res;
    

	switch (command) {
        case ACTION_SHOW_FIXTURE : 
		res_fixture_t* r = res_fixture(ipc, db, msg->sender);
		res.content=r;
            break;
        //case ACTION_GET_TICKETS :
        //res_buy_ticket_t* r =res_buy_ticket(ipc, database, msg->sender, (req_buy_tickets_t*) req);
	//res.content;
        //    break;
        case ACTION_BUY_TICKETS :
            res_buy_ticket_t* r = res_buy_ticket(ipc, db, msg->sender, (req_buy_tickets_t*) req);
	    res.content=r;
            break;
        default: 
            res_error_t* r = res_error(ipc,msg->sender,ERR_INVALID_COMMAND);
	    res.content=r;
            break;
    }
	//Creamos un ipc nuevo entre el worker y el cliente, cuyo id es el pid del cliente guardado en el msg.
	ipc = ipc_create(msg.sender);
	res.serder=getpid();
        ipc_send(&ipc, res);
}

static uint8_t command_interpreter(message_t msg) {
    return *((uint8_t *) msg ); //all de request structs begin with a uint8_t type which represents de type of request!
}
