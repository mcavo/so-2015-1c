#include <stdio.h>
#include <string.h>
#include "../inc/actions.h"

/* conectar la base del cliente Luego va a ir del lado de los workes */


/* función repetida.. no se donde ponerla */
BOOL validRange( int start[2], int end[2], sala_t * sala ) {
    int start_p = get_position(start[0], start[1]);
    int end_p = get_position(end[0], end[1]);
    int i;
    if(end_p >= start_p && end_p < MAX_PLACES) {
        for(i=start_p; i <= end_p; i++) {
            if (sala->places[i/MAX_COL][i%MAX_COL] == '1')
                return FALSE;
        }
        return TRUE;
    }
    return FALSE;
}

void markAsSelected(int start, int end, sala_t * sala) {
    int i;
    for(i=start; i <= end ; i++) {
        if(i >= start && i <= end) {
            sala->places[i/MAX_COL][i%MAX_COL]='2';
        }
    }
}
