#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define MAX_MOVIE_TITLE 20
#define MAX_SALA 20

#define MAX_LARGE 2

#define MAX_RESERVA 15
#define MAX_COL 6
#define MAX_ROW 7
#define MAX_PLACES MAX_COL*MAX_ROW
#define get_position(row, col)  ((row*MAX_COL)+col+1)
#define invalid_ticket_interval(start, end) (end>start)? 1:0

/* state machine for readind film text */
#define ROW_STATUS 0
#define COL_STATUS 1
#define SITS_STATUS 2
#define END_STATUS 3

#define INVALID_INTERVAL -2
#define OCCUPIED_SEATS -1
#define SUCCESFUL_OPERATION 0

#define BOOL int
#define TRUE 1
#define FALSE 0
            	
typedef struct {
	char* name;
	int cols;
	int rows;
	int* places; /* si la cantidad es inferior, el resto se completa con -1 */
} sala_t;

typedef struct {
	char* movie_name;
	int* start;
	int* end;
} booking_t; 

typedef struct {
	char** titles;
	int count; /* cantidad de películas a mostrar*/
}  fixture_t;

void confirm_booking (booking_t booking);
fixture_t * get_movies();
sala_t get_sala(char* pelicula);
int buy_tickets(booking_t booking);
