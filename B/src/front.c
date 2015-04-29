#include "../inc/front.h"
#include <ctype.h>

#define clean while(getchar()!='\n')

static ticket_t getPosition (char row, int col);
static ticket_t askPosition (char *msg);
//REMOVE: static void handle_res(ipc_t *ipc_res);
static void handle_res();

//REMOVE: void actionShowFixture(ipc_t *ipc, ipc_t *ipc_res) {
void actionShowFixture(ipc_t *ipc) {
	printf("actionnnn\n");
	req_fixture(ipc);
	//REMOVE: handle_res(ipc_res);
	handle_res();
}

//RMEOVE: void actionBuyTickets(ipc_t *ipc, ipc_t *ipc_res) {
void actionBuyTickets(ipc_t *ipc) {
	uint16_t movie_id;
	printf("  Select choose the movie code: [NUM]\n   ");
	scanf("%d",(int*)(&movie_id));
	clean;
	ticket_t first = askPosition("Please choose the first position you want to buy. [ROW COL]");
	printf("%d\n",first);
	ticket_t last = askPosition("Please choose the last position you want to buy. [ROW COL]");
	printf("%d\n",last);
	req_buy_tickets(ipc,movie_id-1,first,last);
	//REMOVE: handle_res(ipc_res);
	handle_res();
}

//REMOVE: void actionPrintCinema(ipc_t *ipc, ipc_t *ipc_res) {
void actionPrintCinema(ipc_t *ipc) {
	uint16_t movie_id;
	printf("  Select choose the movie code: [NUM]\n   ");
	scanf("%d",(int*)(&movie_id));
	clean;
	req_print_cinema(ipc, movie_id-1);
	//REMOVE: handle_res(ipc_res);
	handle_res();
}

static ticket_t askPosition(char *msg) {
	char row;
	int col;
	printf("  %s\n   ",msg);
	scanf("%c %d",&row,&col);
	clean;
	return getPosition(row,col);
}

static ticket_t getPosition (char row, int col) {
	int ri = (int) (tolower(row) -'a');
	int ci = col - 1;
	return ri*MOVIE_MAX_COL+ci;
}

//REMOVE: static void handle_res(ipc_t *ipc) {
static void handle_res() {
	//ADDED:
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
