#ifndef __FRONT_H__
#define __FRONT_H__

#include "actions.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>

void actionShowFixture(ipc_t *ipc);
void actionBuyTickets(ipc_t *ipc);
void actionPrintCinema(ipc_t *ipc);

#endif
