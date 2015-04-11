#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define MAX_RESERVA 15
#define MAX_COL 6
#define MAX_ROW 7
#define MAX_PLACES MAX_COL*MAX_ROW
#define get_position(row, col)  ((row*MAX_COL)+col+1)

struct {
	int cols;
	int rows;
	int[MAX_PLACES] places; /* si la cantidad es inferior, el resto se completa con -1 */
} sala_t;

struct {
	char* movie_name;
	//int continuos;
	//int[15][2] asientos;
	int[2] start;
	int[2] end;
}booking_t
