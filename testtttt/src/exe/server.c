#include "../../inc/actions.h"
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>


static void worker(message_t * msg);
static uint8_t command_interpreter(char* msg);

database_t *db;


void save_server_pid(int server_pid){
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
	ipc_t *ipc;   

    //Dejamos en un archivo el pid del servidor para que los clientes lo tomen y se comuniquen. 
    save_server_pid(server_pid);

    //Creamos el ipc unico, cuyo id es el pid del servidor.
    ipc = ipc_open(server_pid); 	
	
	printf("server_pid: %d ipc_id: %d\n", server_pid,ipc->id); 

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
					worker(cmd);//muere solo


                case -1:
                    perror("fork2");
                    exit(1);
                default:
                    exit(0);//El padre del nieto (hijo) termina inmediatamente. En el default del fork de afuera esta esperando esta rta.
            }
        default:
            waitpid(pid,NULL,0);
        }
    
    }
        return 0;
}

static void worker(message_t * msg) {
	printf("llego al worker!\n");    
	printf("msg.sender en worker: %d\n",msg->sender);
	printf("msg.content en server: %d\n",((req_fixture_t*)(msg->content))->type);
	uint8_t command = command_interpreter(msg->content);
    ipc_t* ipc;
    message_t res;
    char * r;

	switch (command) {
        case ACTION_SHOW_FIXTURE: 
			r = (char*)res_fixture(db);
			memcpy(res.content,r,*((uint32_t*)(r+sizeof(uint8_t))));
            break;
        case ACTION_PRINT_CINEMA:
			r = (char*)res_print_cinema(db, (req_print_cinema_t*) msg->content);
			memcpy(res.content,r,*((uint32_t*)(r+sizeof(uint8_t))));
			break;
        case ACTION_BUY_TICKETS:
			r = (char*)res_buy_tickets(db,(req_buy_tickets_t*) msg->content);
			memcpy(res.content,r,*((uint32_t*)(r+sizeof(uint8_t))));
            break;
        default: 
			r = (char*)res_error(ERR_INVALID_COMMAND);
			memcpy(res.content,r,*((uint32_t*)(r+sizeof(uint8_t))));
            break;
    }
	//Creamos un ipc nuevo entre el worker y el cliente, cuyo id es el pid del cliente guardado en el msg.
  	
	ipc = ipc_connect(msg->sender);
	printf("Conecto worker con ipc_res id: %d\n",ipc->id);  
	
	res.sender=getpid();
    ipc_send(ipc, &res, sizeof(res));
}

static uint8_t command_interpreter(char* cmd) {
    return *((uint8_t *) cmd ); //all de request structs begin with a uint8_t type which represents de type of request!
}
