#ifndef __DBCONNECTOR_H__
#define __DBCONNECTOR_H__

#define MOVIE_MAX_ROW 7
#define MOVIE_MAX_COL 6
#define MOVIE_MAX_PLACES MOVIE_MAX_ROW*MOVIE_MAX_COL

#define MOVIE_NAME_LENGTH 30

#define ERR_NO_SUCH_MOVIE -1
#define ERR_OCCUPIED_SEATS -2
#define ERR_NO_MORE_TICKETS -3

typedef uint8_t ticket_t;

typedef struct movie {
	char name[MOVIE_NAME_LENGTH];
	ticket_t tickets[MOVIE_MAX_PLACES];
} movie_t;

typedef struct database {
	int fd;
	struct stat st;
	uint16_t count;
	movie_t* movies;
} database_t;

void db_movie_init(movie_t* movie, char* name);

database_t *db_open(char *path);

void db_close(database_t *db);

BOOL db_buy_tickets (database_t *db, uint16_t movie_id, int first, int last);
ticket_t* db_get_tickets(database_t *db, uint16_t movie_id);

#endif