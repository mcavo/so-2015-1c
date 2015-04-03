#define TICKET_FREE         0
#define TICKET_BOUGHT       1

typedef uint8_t ticket_t;

typedef struct movie {
    char* name;
    uint16_t size;
    uint16_t remaining_tickets;
    ticket_t* tickets;
} movie_t;


void action_buy_tickets();
