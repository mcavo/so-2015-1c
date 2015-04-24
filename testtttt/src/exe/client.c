#include "../../inc/front.h"

void handle_response(ipc_t *ipc);

void showCinemaTitle ();

int showMenu();

int main(int argc, char** argv) {
	//check(argc >= 2, "Usage: client <server address> <action> [params...]\n");
	char *server = argv[1];
	int action;
	
	//int action_code = action_string_to_code(action);
	//check(action_code, "Unknown action: %s\n", action);
	/* Valid arguments, valid command. Let's connect */
	ipc_t *ipc = ipc_connect(server);
	while ( (action = showMenu())!=ACTION_EXIT ) {
		switch (action_code) {
			case ACTION_SHOW_FIXTURE:
				//check(argc == 3, "Usage: client <server address> list\n");
				actionShowFixture(ipc);
				break;
			case ACTION_BUY_TICKETS:
				//check(argc == 4, "Usage: client <server address> buy <movie id>\n");
				//int movie_id = atoi(argv[3]);
				actionBuyTickets(ipc);
				break;
			case ACTION_INVALID:
				printf("Invalid option.\n");
				break;
		}
	}
	printf("\n\nGoodbye!\n");
}

void showCinemaTitle () {
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

int showMenu(){
    int choice = -1;
        printf("\n  **********\n\
  ** MENU **\n\
  **********\n\n\
  1.Show fixture\n\
  2.Buy tickets\n\
  3.Exit\n\n\
  Please, select your choice:\n\
    ");
  scanf("%d", &choice);
  getchar();

  return choice;
}

void handle_response(ipc_t *ipc) {
	message_t *msg = ipc_receive(ipc);
	res_any_t *res = (void*) &(msg->content);
	switch (res->type) {
		case ACTION_MOVIE_LIST:
			hand_movie_list((res_movie_list_t*) res);
			break;
		case ACTION_BUY_TICKET:
			hand_buy_ticket((res_buy_ticket_t*) res);
			break;
		case ACTION_ERROR:
			hand_error((res_error_t*) res);
			break;
	}
	free(msg);
}