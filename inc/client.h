#define ANSI_COLOR_BOUGHT	"\e[1;31m"	//RED
#define ANSI_COLOR_SELECTED	"\e[1;32m"	//GREEN
#define	ANSI_COLOR_RESET	"\e[1;0m"	//resetea el color, sino en la consola se sigue imprimiendo en el color seteado
#define	ANSI_COLOR_AVAILABLE "\e[1;0m"		//Default

#define CANCELLED_OPERATION 1
#define OUT_OF_MEMORY -4

#include <stdio.h>
#include <string.h>
#include "actions.h"

void actionShowMovies();
void actionBuyTickets();

void printMsg (int code);
