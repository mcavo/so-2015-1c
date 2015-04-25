#include "../inc/front.h"
#include <ctype.h>

static void actionPrintCinema(ipc_t *ipc, uint16_t movie_id);
static ticket_t getPosition (char row, int col);
static ticket_t askPosition (char *msg);
static void handle_res(ipc_t *ipc);

void actionShowFixture(ipc_t *ipc) {
	req_fixture(ipc);
	handle_res(ipc);
	ipc_close(ipc);
}

void actionBuyTickets(ipc_t *ipc) {
	uint16_t movie_id;
	actionShowFixture(ipc);
	printf("  Select your choise, please: [NUM]\n");
	fflush(stdin);
	scanf("%d",(int*)&movie_id);
	actionPrintCinema(ipc,movie_id-1);
	ticket_t first = askPosition("Please choose the first position you want to buy. [ROW COL]");
	ticket_t last = askPosition("Please choose the last position you want to buy. [ROW COL]");
	req_buy_tickets(ipc,movie_id-1,first,last);
	handle_res(ipc);
	ipc_close(ipc);
}

static void actionPrintCinema(ipc_t *ipc, uint16_t movie_id) {
	req_print_cinema(ipc, movie_id);
	handle_res(ipc);
	ipc_close(ipc);
}

static ticket_t askPosition(char *msg) {
	char row;
	int col;
	printf("  %s\n   ",msg);
	scanf("%c %d",&row,&col);
	fflush(stdin);
	return getPosition(row,col);
}

static ticket_t getPosition (char row, int col) {
	int ri = (int) (tolower(row) -'a');
	int ci = col - 1;
	return ri*MOVIE_MAX_COL+ci;
}

static void handle_res(ipc_t *ipc) {
	message_t *msg = ipc_receive(ipc);
	res_any_t *res = (void*) &(msg->content);
	switch (res->type) {
		case ACTION_SHOW_FIXTURE:
			hand_fixture((res_fixture_t*) res);
			break;
		case ACTION_BUY_TICKETS:
			hand_buy_tickets((res_buy_tickets_t*) res);
			break;
		case ACTION_PRINT_CINEMA:
			hand_print_cinema((res_print_cinema_t*) res);
			break;
		case ACTION_ERROR:
			hand_error((res_error_t*) res);
			break;
	}
	free(msg);
}