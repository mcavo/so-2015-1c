#include "../../inc/front.h"


static void showCinemaTitle();

static int showMenu();




int main(int argc, char** argv) {

	int action;
	int server_pid = get_server_pid();

	if(server_pid==-1){
		printf("Error server_pid.txt\n");
		exit(0);
	}	
	
	showCinemaTitle();

	ipc_t *ipc = ipc_connect(server_pid);
	//REMOVE: ipc_t *ipc_res;
	int pid=getpid();


	while ( (action = showMenu())!=ACTION_EXIT ) {

		//REMOVE: ipc_res = ipc_listen(pid);
		//REMOVE: printf("Cliente abre ok ipc_res id: %d, pid_cliente: %d\n",ipc_res->id ,pid); 
		printf("Cliente abre ok pid_cliente: %d\n", pid); 

		switch (action) {
			case ACTION_SHOW_FIXTURE:				
				//REMOVE: actionShowFixture(ipc, ipc_res);
				actionShowFixture(ipc);
				break;
			case ACTION_BUY_TICKETS:
				//REMOVE: actionBuyTickets(ipc, ipc_res);
				actionBuyTickets(ipc);
				break;
			case ACTION_PRINT_CINEMA:
				//REMOVE: actionPrintCinema(ipc, ipc_res);
				actionPrintCinema(ipc);
			default:
				printf("Invalid option.\n");
				break;
		}
	}
	printf("\n\nGoodbye!\n");

	return 0;
}

static void showCinemaTitle () {
    printf("\n\
                ****  ***********     ********\n\
                ****  ************   **********\n\
                ****  ****    ****   ****   ***\n\
                ****  ****    ****   ***\n\
                ****  ************   ***\n\
                ****  ***********    ***\n\
                ****  ****           ****   ***\n\
                ****  ****            *********\n\
                ****  ****             *******\n\
                ****\n\
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

static int showMenu(){
    int choice = -1;
        printf("\n  **********\n\
  ** MENU **\n\
  **********\n\n\
  1.Show fixture\n\
  2.Buy tickets\n\
  3.Check sala\n\
  4.Exit\n\n\
  Please, select your choice:\n\
    ");
  scanf("%d", &choice);

  return choice;
}


