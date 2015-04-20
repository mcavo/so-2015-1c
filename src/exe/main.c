
#include "../../inc/client.h"


#define ACTION_SHOW_FIXTURE 1
#define ACTION_BUY_TICKETS  2
#define ACTION_EXIT         3
#define ACTION_INVALID      4






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
    return 0;
}

