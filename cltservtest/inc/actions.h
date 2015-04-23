#ifndef __ACTIONS_H__
#define __ACTIONS_H__

#include "ipc.h"
#include "database.h"
#include "utils.h"

#define ACTION_SHOW_FIXTURE 1
#define ACTION_BUY_TICKETS  2
#define ACTION_EXIT         3
#define ACTION_INVALID      4

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

void han_error(res_error_t *err);

typedef struct {
	uint8_t type;
} req_movie_list_t;

typedef struct {
	uint8_t type;
	uint16_t count;
	movie_t movies[];
} res_movie_list_t;

void req_movie_list(ipc_t *ipc);
void res_movie_list(ipc_t *ipc, database_t *db, uint16_t sender);
void han_movie_list(res_movie_list_t *res);

typedef struct {
	uint8_t type;
	uint16_t movie_i;
} req_buy_ticket_t;

typedef struct {
	uint8_t type;
	ticket_t ticket;
} res_buy_ticket_t;

void req_buy_ticket(ipc_t *ipc, uint16_t movie_i);
void res_buy_ticket(ipc_t *ipc, database_t *db, uint16_t sender, req_buy_ticket_t *req);
void han_buy_ticket(res_buy_ticket_t *res);

#endif