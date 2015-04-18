#include "db_connector.h"


#define CONFIRM 0
#define DONT_CONFIRM 1
#define QUIT 2

#define TICKET_FREE         0
#define TICKET_BOUGHT       1
#define MAX_MOVIE_LENGHT    50

void markAsSelected(int start, int end, sala_t * sala);

BOOL validRange( int* start, int* end, sala_t * sala );