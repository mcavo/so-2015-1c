#define TAKEN               '\e[0;31m' //RED
#define SELECTED            '\e[1;32m' //GREEN
#define FREE                ' ' //Default HAy que ver como solucionamos esto. Sino no compila

#include <stdio.h>
#include <string.h>
#include "actions.h"

fixture_t actionShowMovies();
int action_buy_tickets();