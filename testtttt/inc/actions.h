#ifndef __ACTIONS_H__
#define __ACTIONS_H__

#include "ipc.h"
#include "dbconnector.h"
#include <stdio.h>

#define ACTION_SHOW_FIXTURE 1
#define ACTION_BUY_TICKETS 2
#define ACTION_GET_TICKETS 3
#define ACTION_ERROR 59

#define ANSI_C_ERROR_COLOR "\e[1;31m" //RED
#define ANSI_C_OK_COLOR "\e[1;32m" //GREEN
#define ANSI_C_RESET_COLOR "\e[1;0m"

#define TICKET_AVAILABLE "\e[1;31m" //RED
#define TICKET_BOUGHT "\e[1;32m" //GREEN

typedef struct {
	uint8_t type;
} req_any_t;

typedef struct {
	uint8_t type;
} res_any_t;

typedef struct {
	uint8_t type;
	int32_t code;
} res_error_t;

void res_error(ipc_t *ipc, uint16_t sender, int32_t code);
void hand_error(res_error_t *err);

typedef struct {
	uint8_t type;
} req_fixture_t;

typedef struct {
	uint8_t type;
	uint16_t count;
	movie_t movies[];
} res_fixture_t;

void req_fixture(ipc_t *ipc);
void res_fixture(ipc_t *ipc, database_t *db, uint16_t sender);
void hand_fixture(res_fixture_t *res);

typedef struct {
	uint8_t  ;
	uint16_t movie_id;
	ticket_t first;
	ticket_t last;
} req_buy_tickets_t;

typedef struct {
	uint8_t end;
} res_buy_tickets_t;

void req_buy_tickets(ipc_t *ipc, uint16_t movie_id, ticket_t first, ticket_t last);
void res_buy_tickets(ipc_t *ipc, database_t *db, uint16_t sender, req_buy_tickets_t *req);
void hand_buy_tickets(res_buy_ticket_t *res);
/*
typedef struct {
	uint8_t type;
	uint16_t movie_id;
} req_get_tickets_t;

typedef struct {
	uint8_t type;
	uint8_t tickets[];
} res_get_tickets_t;

void req_get_tickets(ipc_t *ipc, uint16_t movie_id);
void res_get_tickets(ipc_t *ipc, database_t *db, uint16_t sender, req_get_ticket_t *req);
void hand_get_tickets(res_get_ticket_t *res);
*/
#endif