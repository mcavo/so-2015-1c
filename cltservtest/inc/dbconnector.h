#ifndef __DBCONNECTOR_H__
#define __DBCONNECTOR_H__

#include <stdint.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>

#define MOVIE_MAX_COL 6
#define MOVIE_MAX_ROW 7
#define MOVIE_MAX_PLACES MOVIE_MAX_COL*MOVIE_MAX_ROW
#define MOVIE_NAME_LENGTH 30
#define MOVIE_TIME_LENGTH 100

#define ERR_NO_SUCH_MOVIE -1
#define ERR_OCCUPIED_SEATS -2
#define ERR_NO_MORE_TICKETS -3

typedef int32_t ticket_t;

typedef struct movie {
	char name[MOVIE_NAME_LENGTH];
	uint16_t remaining_tickets;
	ticket_t tickets[MOVIE_PLACES];
} movie_t;

typedef struct database {
	FILE* file;
	struct stat st;
	uint16_t count;
	movie_t* movies;
} database_t;

void movie_init(movie_t* movie, char* name, char* time);
database_t *db_open(char *path);
void db_close(database_t *database);
void db_rlock(database_t *database);
void db_wlock(database_t *database);
void db_unlock(database_t *database);

movie_t* db_find(database_t *database, char* name);
ticket_t db_buy_ticketS (database_t *database, int movie_i);

void db_read_fix(char *database_path, char *fixtures_path);

#endif