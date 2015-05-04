#include "../inc/front.h"
#include "../inc/utils.h"
#include <ctype.h>

static ticket_t getPosition (char row, int col);
static ticket_t askPosition (char *msg);
static void handle_res();

void actionShowFixture(ipc_t *ipc) {
	req_fixture(ipc);
	handle_res();
}

void actionBuyTickets(ipc_t *ipc) {
	uint16_t movie_id;
	req_fixture(ipc);
	handle_res();
	printf("\n  Please choose the movie code: [NUM]\n    ");
	scanf("%d",(int*)(&movie_id));
	CLEAN_BUFFER
	req_print_cinema(ipc, movie_id-1);
	handle_res();
	ticket_t first = askPosition("Please choose the first position you want to buy. [ROW COL]");
	ticket_t last = askPosition("Please choose the last position you want to buy. [ROW COL]");
	req_buy_tickets(ipc,movie_id-1,first,last);
	handle_res();
}

void actionPrintCinema(ipc_t *ipc) {
	uint16_t movie_id;
	req_fixture(ipc);
	handle_res();
	printf("\n  Please choose the movie code: [NUM]\n    ");
	scanf("%d",(int*)(&movie_id));
	CLEAN_BUFFER
	req_print_cinema(ipc, movie_id-1);
	handle_res();
}

static ticket_t askPosition(char *msg) {
	char row;
	int col;
	printf("  %s\n    ",msg);
	scanf("%c %d",&row,&col);
	CLEAN_BUFFER
	return getPosition(row,col);
}

static ticket_t getPosition (char row, int col) {
	int ri = (int) (tolower(row) -'a');
	int ci = col - 1;
	return ri*MOVIE_MAX_COL+ci;
}

static void handle_res() {
	printf("%s\n","hand_res");
	ipc_t *ipc = ipc_listen(getpid());
	message_t *msg = ipc_receive(ipc);
	uint8_t t = (uint8_t) (*(msg->content));
	switch (t) {
		case ACTION_SHOW_FIXTURE:
			hand_fixture((res_fixture_t*) (msg->content));
			break;
		case ACTION_BUY_TICKETS:
			hand_buy_tickets((res_buy_tickets_t*) (msg->content));
			break;
		case ACTION_PRINT_CINEMA:
			hand_print_cinema((res_print_cinema_t*) (msg->content));
			break;
		case ACTION_ERROR:
			hand_error((res_error_t*) (msg->content));
			break;
	}
	free(msg);
	ipc_close(ipc);
}
