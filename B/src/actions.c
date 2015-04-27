#include "../inc/actions.h"


void res_error(ipc_t *ipc,uint16_t sender, int32_t code) {
	res_error_t res = {
		.type = ACTION_ERROR,
		.code = code
	};
	ipc_send(ipc, sender, &res, sizeof(res));
	ipc_close(ipc);
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

int get_server_pid(){

	FILE *fp;
	char str[60];
	int pid;
	fp = fopen("serverPid.txt" , "r");

	if(fp == NULL) 
	{
	perror("Error opening file");
		return -1;
	}

	if( fgets (str, 60, fp)==NULL ) 
		return -1;

	pid= atoi(str);

	fclose(fp);

	return pid;
   
}


void req_fixture(ipc_t *ipc) {
	req_fixture_t req = {
		.type = ACTION_SHOW_FIXTURE
	};
	printf("direccion del req original req_fixture: %d\n", (int)(&req));
	ipc_send(ipc, ipc->server_id, &req, sizeof(req));

}

void res_fixture(ipc_t *ipc, database_t *db,uint16_t sender){
	printf("%d\n",db->count );
	size_t fix_size = sizeof(movie_t) * db->count;
	size_t res_size = sizeof(res_fixture_t) + fix_size;
	res_fixture_t *res = (res_fixture_t*) malloc(res_size);
	res->type = ACTION_SHOW_FIXTURE;
	res->count = db->count;
	memcpy(res->movies, db->movies, fix_size);
    ipc_send(ipc, sender, res, res_size);
    printf("%d\n",(int)res);
    free(res);
    ipc_close(ipc);
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
	
	ipc_send(ipc, ipc->server_id, &req, sizeof(req));
}

void res_buy_tickets(ipc_t *ipc, database_t *db,uint16_t sender, req_buy_tickets_t *req) {
	int error = db_buy_tickets(db,req->movie_id,req->first,req->last);
	if(error<0) {
		res_error(ipc,sender, error);
		return;
	}
	res_buy_tickets_t res = {
		.type = ACTION_BUY_TICKETS
	};
	ipc_send(ipc, sender, &res, sizeof(res));
	ipc_close(ipc);
}

void hand_buy_tickets(res_buy_tickets_t *res) {
	printf("  %sSuccesful operation!%s\n",ANSI_C_OK_COLOR,ANSI_C_RESET_COLOR);
}



void req_print_cinema(ipc_t *ipc, uint16_t movie_id) {
	req_print_cinema_t req = {
		.type = ACTION_PRINT_CINEMA,
		.movie_id = movie_id
	};
	
	ipc_send(ipc, ipc->server_id, &req, sizeof(req));
}

void res_print_cinema(ipc_t *ipc, database_t *db, uint16_t sender, req_print_cinema_t *req) {
	ticket_t * cinema;
	int error = db_get_cinema(db,req->movie_id,&cinema);
	if(cinema == NULL) {
		res_error(ipc,sender,error);
	}
	else {
		int size = sizeof(uint8_t)+sizeof(ticket_t)*MOVIE_MAX_PLACES;
		res_print_cinema_t res;
		res.type = ACTION_PRINT_CINEMA;
		memcpy(res.tickets,cinema,MOVIE_MAX_PLACES);
		free(cinema);
		ipc_send(ipc, sender, &res, size);
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
	for (i=1 ; i <= MOVIE_MAX_COL ; i++) {
		printf("%d ",i);
	}
	printf("\n\n");
}


