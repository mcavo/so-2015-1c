
#include "../../inc/client.h"
#include "../../inc/filesig.h"

#define ACTION_SHOW_FIXTURE 1
#define ACTION_BUY_TICKETS  2
#define ACTION_EXIT         3
#define ACTION_INVALID      4




void test_ipc(){
	pid_t server_pid;
	FILE *fd = fopen(SERVER, "r+");
	fscanf(fd, "%d", &server_pid);
	fclose(fd);

	/* ipc creation */
	ipc_t *ipc_l = ipc_listen(getpid());
	ipc_t *ipc_c = ipc_connect(server_pid);

	/* send message */
	message_t *msg = (message_t *)malloc(sizeof(message_t));
	msg -> content = "Hola";
	msg -> length = 5;
	msg -> from = getpid();
	ipc_send(msg, ipc_c);
	free(msg);

	/* read message */
	msg = (message_t *)malloc(sizeof(message_t));
	msg = ipc_read(ipc_l);
	printf("%s\n", msg->content);
	free(msg);
}

void showCinemaTitle () {
    printf(" *****    ***********      **********\n\
 *****    ************    ************\n\
 *****    ****    *****   ****    ****\n\
 *****    ****    *****   ****\n\
 *****    ************    ****\n\
 *****    ***********     ****\n\
 *****    ****	           ****    ****\n\
 *****    ****	           ************\n\
 *****    ****	            **********\n\n\
  **********    ****  ****    ****   **********   ****     ****       ****\n\
 ************   ****  *****   ****   **********   *****   *****      ******\n\
 ****    ****   ****  ******  ****   ****         ****** ******     ********\n\
 ****           ****  ******* ****   ****         *************    ****  ****\n\
 ****           ****  ************   *******      **** *** ****   ****    ****\n\
 ****           ****  **** *******   *******      ****  *  ****   ****    ****\n\
 ****    ****   ****  ****  ******   ****         ****     ****   ************\n\
 ************   ****  ****   *****   **********   ****     ****   ************\n\
  **********    ****  ****    ****   **********   ****     ****   ****    ****\n\n\n\
    ");


}

int showMenu(){
    int choice = -1;
        printf("\n  **********\n\
  ** MENU **\n\
  **********\n\n\
  1.Show movies\n\
  2.Buy tickets\n\
  3.Exit\n\n\
  Please, select your choice:\n\
    ");
  scanf("%d", &choice);
  getchar();

  return choice;
}


int execRequest(/*ipc_t* ipc*/){
    int choice=0;
    while ((choice=showMenu())!=ACTION_EXIT) {
        switch(choice) {
            case ACTION_BUY_TICKETS: actionBuyTickets() ; break;
            case ACTION_SHOW_FIXTURE: actionShowMovies() ; break;
            case ACTION_EXIT: printf(" Come back soon!\n"); break;
            default: printf("Invalid argument:\n %d is not a valid choice.\n\n",choice); break;
        }
    }
    return 0;
}

int main() {
    showCinemaTitle();
    execRequest();
	test_ipc();
    return 0;
}

