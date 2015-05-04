
#include "../../inc/client.h"


#define ACTION_SHOW_FIXTURE 1
#define ACTION_BUY_TICKETS  2
#define ACTION_PRINT_CINEMA 3
#define ACTION_EXIT         4
#define ACTION_INVALID      5






void showCinemaTitle () {
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

int showMenu(){
    int choice = -1;
        printf("\n  **********\n\
  ** MENU **\n\
  **********\n\n\
  1.Show movies\n\
  2.Buy tickets\n\
  3.Print cinema\n\
  4.Exit\n\n\
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
	    case ACTION_PRINT_CINEMA: actionPrintCinema() ; break;
            case ACTION_EXIT: break;
            default: printf("Invalid argument:\n %d is not a valid choice.\n\n",choice); break;
        }
    }
    printf("\n\n  Come back soon!\n\n");
    return 0;
}

int main() {
    showCinemaTitle();
    execRequest();
    return 0;
}

