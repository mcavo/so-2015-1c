#define ANSI_COLOR_BOUGHT	"\e[1;31m"	//RED
#define ANSI_COLOR_SELECTED	"\e[1;32m"	//GREEN
#define	ANSI_COLOR_RESET	"\e[1;0m"	//resetea el color, sino en la consola se sigue imprimiendo en el color seteado
#define	ANSI_COLOR_AVAILABLE "\e[1;0m"		//Default


#include <stdio.h>
#include <string.h>
#include "actions.h"

int actionShowMovies();
int actionBuyTickets();
