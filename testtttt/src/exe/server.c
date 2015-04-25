#include "actions.h"
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

static void worker(message_t * msg);
static uint8_t command_interpreter(message_t msg)

database_t *db;
ipc_t *ipc;

int main(void)
{
    pid_t pid;
    message_t* cmd;

    ipc= ipc_open("server.c"); 	

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
    switch (command) {
        case ACTION_SHOW_FIXTURE : 
            res_fixture(ipc, db, msg->sender);
            break;
        //case ACTION_GET_TICKETS :
        //    res_buy_ticket(ipc, database, msg->sender, (req_buy_tickets_t*) req);
        //    break;
        case ACTION_BUY_TICKETS :
            res_buy_ticket(ipc, db, msg->sender, (req_buy_tickets_t*) req);
            break;
        default: 
            res_error(ipc,msg->sender,ERR_INVALID_COMMAND);
            break;
    } 
}

static uint8_t command_interpreter(message_t msg) {
    return *((uint8_t *) msg ); //all de request structs begin with a uint8_t type which represents de type of request!
}
