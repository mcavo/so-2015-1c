/* actions client - front */

#include "../inc/client.h"
#include <ctype.h>

/* print sala with diferent colors (selected, bought, available) */
void printSala (sala_t sala) {
    int i;
    char* color;
    for(i=0;i<sala.rows*sala.cols;i++) {
        if(sala.places[i]==1)
            color=ANSI_COLOR_BOUGHT;
        else {
            if(sala.places[i]==2)
                color=ANSI_COLOR_SELECTED;
            else
                color=ANSI_COLOR_AVAILABLE;
        }
        if (i%sala.cols == 0)
            printf("\n");
        printf("%sO%s",color,ANSI_COLOR_RESET);
    }
}

char* getMovieCode(){
	int index;
	fixture_t fixture = actionShowMovies();
	do{
		printf("%s\n", "Seleccione el número de película que desea ver");
		scanf("%i", &index); 
	} while(!(0 < index && index <= fixture.count));
	return *(fixture.titles + index - 1);
}

void copyPlacesArray(int* first, int* second){
	int i;
    for (i = 0; i < MAX_PLACES; i++){
		*(second + i) = *(first + i);
	}
}

/* get positon of the a booking sit */
void getPosition(int* pos, char * msg) {
    char cfil;
    int col;
    printf("%s\n", msg);
    scanf("%c%d",&cfil,&col);
    pos[0]=((int) (cfil-'a')) - 1;
    pos[1]=col - 1;
}

/* ask confirmation */
BOOL askConfirmation(sala_t sala, booking_t b) {
    char c;
    int* aux = malloc(sizeof(int) * MAX_PLACES);

    int start_p = get_position(b.start[0], b.start[1]);
    int end_p = get_position(b.end[0], b.end[1]);

    copyPlacesArray(sala.places, aux);

    markAsSelected(sala, start_p, end_p);

    printSala(sala); //este print no debería estar acá.
    // PREGUNTAR SI CONFIRMA
    //SPANGLISH ALERT! 
    do {
    printf("%s\n", "Desea confirmar su reserva? (S/N)");
    scanf("%c", &c);
    } while(tolower(c)!='n'&&tolower(c)!='s'&&printf("\nPor favor ingrese una S o una N.")); 
    switch(c){
    	case 's':
    	case 'S':
    		return TRUE;
    		break;
    	case 'N':
    	case 'n':
    		sala.places = aux;
    		return FALSE;
    		break;
    }
    return FALSE; //hay que ver si esto está realmente bien. Si ingresa otra cosa
}

/* show sala and ask the sits to book */
booking_t getBooking(char* movie_name) {
	sala_t sala = get_sala (movie_name);
	int* start = malloc(2*sizeof(int));
	int* end = malloc(2*sizeof(int));
	booking_t b;
	b.movie_name=movie_name;

	do {
        do {
            printSala(sala);
            getPosition(start,"\nPlease choose the first position you want to buy\n");
            getPosition(end,"\nPlease choose the last position you want to buy\n");
        } while ( !validRange(start, end, sala) ); //habría que ver si podemos dar un poco más de feedback para saber caul es el problema
        b.start=start;
        b.end=end;
	} while ( askConfirmation(sala, b) );

	return b;
}

/* Show the list of films */
fixture_t actionShowMovies() {
    int i = 0;
    fixture_t movies = get_movies();

    printf("\n  MOVIES LIST \n\
  -----------\n");
    
    for ( ; i < movies.count; i++) {
        printf("%i- %s\n", i + 1, movies.titles[i]);
    }
    
    return movies;
}

//Retorna 0 si la operacion fue exitosa y -1 sino. En el front se trata la accion a seguir.
int actionBuyTickets() {
	char* cod = getMovieCode();
	booking_t booking = getBooking(cod);
	return buy_tickets(booking);
}
