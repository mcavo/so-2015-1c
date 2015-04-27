#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#define MAX_MOVIE_TITLE 20
#define MAX_SALA 20

#define MAX_RESERVA 15
#define MAX_COL 6
#define MAX_ROW 7
#define MAX_PLACES MAX_COL*MAX_ROW

#define get_position(row, col)  ((row*MAX_COL)+col)
#define invalid_ticket_interval(start, end) (end<start)? 1:0

#define CHAR_LEN_ROW 3
#define CHAR_LEN_COL 3


#define OCCUPIED '1'

#define DATABASE_ERROR -3
#define INVALID_INTERVAL -2
#define OCCUPIED_SEATS -1
#define SUCCESFUL_OPERATION 0

#define BOOL int
#define TRUE 1
#define FALSE 0
            	
typedef struct {
	char* name;
	int cols; //NO SON NECESARIAS PARA NUESTRA IMPLEMENTACION
	int rows;
	char places [MAX_ROW][MAX_COL];
} sala_t;

typedef struct {
	char* movie_name;
	int start[2];
	int end[2];
} booking_t; 

typedef struct {
	char** titles;
	int count; /* cantidad de películas a mostrar*/
}  fixture_t;

fixture_t * get_movies();
sala_t * get_sala(char* pelicula);
int buy_tickets(booking_t * booking);
