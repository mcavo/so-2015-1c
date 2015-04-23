#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../inc/actions.h"
#include "../inc/ipc.h"
#include "../inc/utils.h"

/* Error handling: */
void res_error(ipc_t *ipc, uint16_t sender, int32_t code) {
	res_error_t err = {
		.type = ACTION_ERROR,
		.code = code
	};
	ipc_send(ipc, sender, &err, sizeof(err));
}

void han_error(res_error_t *err) {
	switch (err->code) {
		case ERR_NO_SUCH_MOVIE:
			printf("No such movie\n");
			break;
		case ERR_NO_SUCH_TICKET:
			printf("No such ticket\n");
			break;
		case ERR_NO_MORE_TICKETS:
			printf("No more tickets available\n");
			break;
		default:
			printf("Unexpected error %d\n", err->code);
			break;
	}
}

/* List movies: */
void req_fixture(ipc_t *ipc) {
	req_fixture_t req = {
		.type = ACTION_MOVIE_LIST
	};
	ipc_send(ipc, ipc->server_id, &req, sizeof(req));
}

void res_fixture(ipc_t *ipc, database_t *db, uint16_t sender) {
	size_t fixture_size = sizeof(movie_t) * db->count;
	size_t res_size = sizeof(res_fixture_t) + fixture_size;
	res_fixture_t *res = (res_ficture_t*) malloc(res_size);
	res->type = ACTION_SHOW_FIXTURE;
	res->count = db->count;
	memcpy(res->movies, db->movies, fixture_size);
	ipc_send(ipc, sender, res, res_size);
	free(res);
}

void han_fixture(res_fixture_t *res) {
	printf("  Fixture\n\
  --------\n");
	for (int i = 0; i < res->count; i++) {
		printf("  %d. %-30s%s\n",
		i + 1, /* Movie index */
		res->movies[i].name, /* Movie title */
		);
	}
}

/* Buy tickets: */
void req_buy_tickets(ipc_t *ipc, uint16_t movie_id, ticket_t first, ticket_t last) {
	req_buy_ticket_t req = {
		.type = ACTION_BUY_TICKET,
		.movie_id = movie_id,
		.first = first,
		.last = last
	};
	ipc_send(ipc, ipc->server_id, &req, sizeof(req));
}

void res_buy_tickets(ipc_t *ipc, database_t *db, uint16_t sender, req_buy_ticket_t *req) {
	BOOL worked = db_buy_ticket(db, req->movie_i);
	if (worked) {
		res_buy_ticket_t res = {
			.type = ACTION_BUY_TICKETS,
			.ticket = ticket
		};
		ipc_send(ipc, sender, &res, sizeof(res));
	} 
	else
		res_error(ipc, sender, ticket); /* `ticket` holds an error code */
}

void han_buy_ticket(res_buy_ticket_t *res) {
	if (res->ticket != 0)
		printf("Ticket: %d\n", res->ticket);
	else
		printf("Tickets not available");
}

