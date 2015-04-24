/*

To summerize: children become defunct until the parent wait()s, unless the parent is ignoring SIGCHLD. Furthermore, children (living or defunct) whose parents die without 
wa<it()ing for them (again assuming the parent is not ignoring SIGCHLD) become children of the init process, which deals with them heavy-handedly.
*/



#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void)
{
    pid_t pid;
    int rv;
    message_t cmd;

    while(1){


    struct my_msgbuf buf;
    int msqid;
    key_t key;
    if ((key = ftok("kirk.c", 'B')) == -1) {
        perror("ftok");
        exit(1);
    }
    if ((msqid = msgget(key, 0644 | IPC_CREAT)) == -1) {
        perror("msgget");
        exit(1);
    }

        receive_command(&cmd);
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

void worker(message_t* cmd){

    cmd->content

    switch(){

        case ACTION_BUY_TICKETS:
            rta = buy_tickets();

    }

    

    id = ipc_open();

    ipc_send(rta);

}
