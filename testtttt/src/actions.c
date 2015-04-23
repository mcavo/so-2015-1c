#include "../inc/actions.h"

void res_error(ipc_t *ipc, uint16_t sender, int32_t code) {
	res_error_t err = {
		.type = ACTION_ERROR,
		.code = code
	};
	ipc_send(ipc, sender, &err, sizeof(err));
}

void hand_error(res_error_t *err) {
	switch (err->code) {
		case ERR_NO_SUCH_MOVIE:
			printf("%s  Error: No such movie.%s\n",ANSI_C_ERROR_COLOR,ANSI_C_RESET_COLOR);
			break;
		case ERR_OCCUPIED_SEATS:
			printf("%s  Error: Occupied seats.%s\n",ANSI_C_ERROR_COLOR,ANSI_C_RESET_COLOR);
			break;
		case ERR_NO_MORE_TICKETS:
			printf("%s  Error: No more tickets available.%s\n",ANSI_C_ERROR_COLOR,ANSI_C_RESET_COLOR);
			break;
		default:
			printf("%s  Error: Unexpected error %d.%s\n",ANSI_C_ERROR_COLOR,err->code,ANSI_C_RESET_COLOR);
			break;
	}
}



void req_fixture(ipc_t *ipc) {
	req_fixture_t req = {
		.type = ACTION_SHOW_FIXTURE
	};
	ipc_send(ipc, ipc->server_id, &req, sizeof(req));
}

void res_fixture(ipc_t *ipc, database_t *db, uint16_t sender){
	size_t fix_size = sizeof(movie_t) * db->count;
	size_t res_size = sizeof(res_fixture_t) + fix_size;
	res_fixture_t *res = (res_fixture_t*) malloc(res_size);
	res->type = ACTION_SHOW_FIXTURE;
	res->count = db->count;
	memcpy(res->movies, db->movies, fix_size);
	ipc_send(ipc, sender, res, res_size);
	free(res);
}

void hand_fixture(res_fixture_t *res) {
	int i;
	printf("  Fixture\n  --------\n");
	for (i = 0; i < res->count; i++) {
		printf("  %d. %s\n",i+1,res->movies[i].name);
	}
	printf("\n");
}




void req_buy_tickets(ipc_t *ipc, uint16_t movie_id, ticket_t first, ticket_t last) {
	req_buy_tickets_t req {
		.type = ACTION_BUY_TICKETS,
		.movie_id = movie_id,
		.first = first,
		.last = last
	};
	ipc_send(ipc, ipc->server_id, &req, sizeof(req));
}

void res_buy_tickets(ipc_t *ipc, database_t *db, uint16_t sender, req_buy_tickets_t *req) {
	res_buy_tickets_t res {
		.end = db_buy_tickets(req->movie_id,req->first,req->first,req->last)
	}
	if(res.end<0)
		res_error(ipc,sender,res.end);
	else
		ipc_send(ipc, sender, &res, sizeof(res));
}

void hand_buy_tickets(res_buy_ticket_t *res) {
	printf("  %sSuccesful operation!%s\n",ANSI_C_OK_COLOR,ANSI_C_RESET_COLOR);
}



void req_get_tickets(ipc_t *ipc, uint16_t movie_id) {
	req_get_ticket_t req {
		.type = ACTION_GET_TICKETS,
		.movie_id = movie_id
	};
	ipc_send(ipc, ipc->server_id, &req, sizeof(req));
}

TODO
void res_get_tickets(ipc_t *ipc, database_t *db, uint16_t sender, req_get_ticket_t *req) {

}

void hand_get_tickets(res_get_ticket_t *res) {
	int i;
	char row = 'A';
	char * color;

	for (i=0 ; i < MOVIE_MAX_PLACES ; i++) {
		if(i%MOVIE_MAX_COL==0)
			printf("\n  %c\t",row++);
		color = TICKET_AVAILABLE;
		if(res->tickets[i]==1)
			color = TICKET_BOUGHT;
		printf("%sO%s ",color,ANSI_C_RESET_COLOR);
	}
	printf("\n\t");
	for (i=0 ; i < MOVIE_MAX_COL ; i++) {
		printf("%d ",i);
	}
	printf("\n\n");
}