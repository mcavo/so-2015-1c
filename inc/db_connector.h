#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define MAX_PLACES 42
#define MAX_RESERVA 15

struct {
	int cols;
	int rows;
	int[MAX_PLACES] places; /* si la cantidad es inferior, el resto se completa con -1 */
} sala_t;

struct {
	int peliculaCod;
	int continuos;
	int[15][2] asientos;
	int[2] inicio;
	int[2] fin; 
}