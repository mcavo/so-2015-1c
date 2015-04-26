#include "../inc/actions.h"


res_error_t * res_error(int32_t code) {
	res_error_t * res = malloc(sizeof(res_error_t));
	res->type = ACTION_ERROR;
	res->code = code;
	return res;
}


void hand_error(res_error_t *err) {
	switch (err->code) {
		case ERR_NO_SUCH_MOVIE:
			printf("%s  Error: No such movie.%s\n",ANSI_C_ERROR_COLOR,ANSI_C_RESET_COLOR);
			break;
		case ERR_OCCUPIED_SEATS:
			printf("%s  Error: Occupied seats.%s\n",ANSI_C_ERROR_COLOR,ANSI_C_RESET_COLOR);
			break;
		case ERR_INVALID_RANGE:
			printf("%s  Error: Invalid range.%s\n",ANSI_C_ERROR_COLOR,ANSI_C_RESET_COLOR);
			break;
		case ERR_INVALID_TICKETS:
			printf("%s  Error: Invalid tickets.%s\n",ANSI_C_ERROR_COLOR,ANSI_C_RESET_COLOR);
			break;
		case ERR_INVALID_COMMAND:
			printf("%s  Error: Invalid command.%s\n",ANSI_C_ERROR_COLOR,ANSI_C_RESET_COLOR);
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
	printf("direccion del req original req_fixture: %d\n", &req);
	message_t msg;
	msg.sender = getpid();
	//msg.content = malloc(sizeof(req));
	//memcpy(msg.content,&req,sizeof(req_fixture_t*));
	msg.content=&req;
//	int type=((req*)msg.content)->type;
	printf("msg content en req_fixture: %d\n",msg.content);
	
	ipc_send(ipc, &msg, sizeof(msg));
}

res_fixture_t * res_fixture(database_t *db){
	size_t fix_size = sizeof(movie_t) * db->count;
	size_t res_size = sizeof(res_fixture_t) + fix_size;
	res_fixture_t *res = (res_fixture_t*) malloc(res_size);
	res->type = ACTION_SHOW_FIXTURE;
	res->count = db->count;
	memcpy(res->movies, db->movies, fix_size);
	res->size = res_size;
	return res;
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
	req_buy_tickets_t req = {
		.type = ACTION_BUY_TICKETS,
		.movie_id = movie_id,
		.first = first,
		.last = last
	};
	
	message_t msg;
	msg.sender = getpid();
	memcpy(msg.content,&req,sizeof(req));
	ipc_send(ipc, &msg, sizeof(msg));
}

res_buy_tickets_t * res_buy_tickets(database_t *db, req_buy_tickets_t *req) {
	int error = db_buy_tickets(db,req->movie_id,req->first,req->last);
	if(error<0)
		return (res_buy_tickets_t *) res_error(error);
	res_buy_tickets_t * res = malloc(sizeof(res_buy_tickets_t));
	if (res == NULL)
		return (res_buy_tickets_t *) res_error(ERR_OUT_OF_MEMORY);
	res->type = ACTION_BUY_TICKETS,
	res->size = sizeof(res_buy_tickets_t);
	return res;
}

void hand_buy_tickets(res_buy_tickets_t *res) {
	printf("  %sSuccesful operation!%s\n",ANSI_C_OK_COLOR,ANSI_C_RESET_COLOR);
}



void req_print_cinema(ipc_t *ipc, uint16_t movie_id) {
	req_print_cinema_t req = {
		.type = ACTION_PRINT_CINEMA,
		.movie_id = movie_id
	};
	
	message_t msg;
	msg.sender = getpid();
	memcpy(msg.content,&req,sizeof(req));
	
	ipc_send(ipc, &msg, sizeof(msg));

}

res_print_cinema_t * res_print_cinema(database_t *db, req_print_cinema_t *req) {
	ticket_t * cinema;
	int i = db_get_cinema(db,req->movie_id,&cinema);
	if(cinema == NULL)
		return (res_print_cinema_t *) res_error(i);
	else {
		int size = sizeof(uint8_t)+sizeof(uint32_t)+sizeof(ticket_t)*MOVIE_MAX_PLACES;
		res_print_cinema_t * res = malloc(size);
		res->type = ACTION_BUY_TICKETS,
		memcpy(res->tickets,cinema,MOVIE_MAX_PLACES);
		free(cinema);
		return res;
	}
}


void hand_print_cinema(res_print_cinema_t *res) {
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


