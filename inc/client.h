#define ANSI_COLOR_BOUGHT	"\x1b[0;31m"	//RED
#define ANSI_COLOR_SELECTED	"\x1b[1;32m"	//GREEN
#define	ANSI_COLOR_RESET	"\x1b[1;0m"	//resetea el color, sino en la consola se sigue imprimiendo en el color seteado
#define	ANSI_COLOR_AVAILABLE	""		//Default

#include <stdio.h>
#include <string.h>
#include "actions.h"

int actionShowMovies();
int actionBuyTickets();
