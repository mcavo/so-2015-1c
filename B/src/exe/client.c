#include "../../inc/front.h"

#define clean while(getchar()!='\n')

static void showCinemaTitle();

static int showMenu();

static int get_server_pid();


int main(int argc, char** argv) {

	int action;
	int server_pid = get_server_pid();

	if(server_pid==-1){
		printf("Error server_pid.txt\n");
		exit(0);
	}	
	
	showCinemaTitle();

	ipc_t *ipc = ipc_connect(server_pid);
	ipc_t *ipc_res;
	int pid=getpid();
	uint16_t movie_id=0;

	while ( (action = showMenu())!=ACTION_EXIT ) {

		ipc_res = ipc_connect(pid);
		printf("Cliente abre ok ipc_res id: %d, pid_cliente: %d\n",ipc_res->id ,pid);  

		switch (action) {
			case ACTION_SHOW_FIXTURE:				
				actionShowFixture(ipc, ipc_res);
				break;
			case ACTION_BUY_TICKETS:
				actionBuyTickets(ipc, ipc_res);
				break;
			case ACTION_PRINT_CINEMA:
				actionPrintCinema(ipc, ipc_res);
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
  clean;

  return choice;
}




