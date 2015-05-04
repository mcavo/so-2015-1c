#include "../../inc/front.h"
#include "../../inc/utils.h"


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
	while ( (action = showMenu())!=ACTION_EXIT ) {
		
		switch (action) {
			case ACTION_SHOW_FIXTURE:
				actionShowFixture(ipc);
				break;
			case ACTION_BUY_TICKETS:
				actionBuyTickets(ipc);
				break;
			case ACTION_PRINT_CINEMA:
				actionPrintCinema(ipc);
				break;
			default:
				printf("Invalid option.\n");
				break;
		}
	}
	printf("\n\n  Goodbye!\n\n");
	return 0;
}

static void showCinemaTitle () {
    printf("\n\e[1;36m\
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
  **********    ****  ****    ****   **********   ****     ****   ****    ****\e[0m\n\n\n\
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
  CLEAN_BUFFER;

  return choice;
}


