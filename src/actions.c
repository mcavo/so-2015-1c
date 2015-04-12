#include <stdio.h>
#include <string.h>
#include "../inc/actions.h"

/* conectar la base del cliente Luego va a ir del lado de los workes */
char * get_fname (char * movie_name) {
    int len = strlen(movie_name); //len: cant de letras del string sin incluir el '\0'
    char * fname;
    if ((fname = malloc (len+5))!=NULL) { // 5: '.''t''x''t''\0'
        int i;
        for(i=0;i<len;i++){
            if(*(movie_name+i)==' ')
                *(fname+i)='_';
            else
                *(fname+i)=*(movie_name+i);
        }
        *(fname+(len++))='.';
        *(fname+(len++))='t';
        *(fname+(len++))='x';
        *(fname+(len++))='t';
        *(fname+(len))=0;
    }
    return fname;
}

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
    for(i=0; (i <= end_p ; i++) {
        if(i >= start_p && i <= end_p) {
            sala.places[i]=2;
        }
    }
}