#include <stdio.h>
#include <string.h>
#include "../inc/actions.h"

/* conectar la base del cliente Luego va a ir del lado de los workes */


/* función repetida.. no se donde ponerla */
BOOL validRange( int* start, int* end, sala_t sala ) {
    int start_p = get_position(start[0], start[1]);
    int end_p = get_position(end[0], end[1]);
    int i;
    if(end_p >= start_p && end_p < MAX_PLACES) {
        for(i=0; i < (end_p - start_p); i++) {
            if (sala.places[start_p + i] == 1)
                return FALSE;
        }
        return TRUE;
    }
    return FALSE;
}

void markAsSelected(sala_t sala, int start, int end) {
    int i;
    for(i=0; i <= end ; i++) {
        if(i >= start && i <= end) {
            sala.places[i]=2;
        }
    }
}
