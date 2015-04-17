#include "db_connector.h"


#define CONFIRM 0
#define DONT_CONFIRM 1
#define QUIT 2

#define TICKET_FREE         0
#define TICKET_BOUGHT       1
#define MAX_MOVIE_LENGHT    50

void markAsSelected(sala_t * sala, int start, int end);

BOOL validRange( int* start, int* end, sala_t * sala );