#include <stdio.h>

#define ACTION_BUY_TICKETS  1
#define ACTION_SHOW_FIXTURE 2
#define ACTION_EXIT         3
#define ACTION_INVALID      4

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

int main() {
    showCinemaTitle();
    int option = -1;
    while (option!=ACTION_EXIT) {
        option = showMenu();
        switch(option) {
            case ACTION_BUY_TICKETS: break;
            case ACTION_SHOW_FIXTURE: break;
            case ACTION_EXIT: printf(" Come back soon!\n"); break;
            default: printf("Invalid argument:\n %d is not a valid option.\n\n",option); break;
        }
    }
    return 0;
}
