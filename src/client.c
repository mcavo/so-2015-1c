/* actions client - front */

#include "../inc/client.h"

/* print sala with diferent colors (selected, bought, available) */
void printSala (sala_t sala) {
    int i;
    char color;
    for(i=0;i<sala.rows*sala.cols;i++) {
        if(sala.places[i]==1)
            color=TAKEN;
        else {
            if(sala.places[i]==2)
                color=SELECTED;
            else
                color=FREE;
        }
        if (i%sala.cols == 0)
            printf("\n");
        printf("%cO",color);
    }
}

char* getMovieCode(){
	int index;
	fixture_t fixture = actionShowMovies();
	do{
		printf("%s\n", "Seleccione el número de película que desea ver");
		scanf("%i", &index); 
	} while(!(0 < index <= fixture.count));
	return *(fixture.titles + index - 1);
}

void copyPlacesArray(int* first, int* second){
	for (int i = 0; i < MAX_PLACES; i++){
		*(second + i) = *(first + i);
	}
}

/* get positon of the a booking sit */
int* getPosition(char * msg) {
    char cfil;
    int col;
    printf("%s\n", msg);
    scanf("%c%d",&cfil,&col);	
    int* rta;
    rta[0]=((int) (cfil-'a')) - 1;
    rta[1]=col - 1;

    return rta;
}

/* ask confirmation */
BOOL askConfirmation(sala_t sala, booking_t b) {
    int i;
    char c;
    int* aux = malloc(sizeof(int) * MAX_PLACES);

    int start_p = get_position(b.start[0], b.start[1]);
    int end_p = get_position(b.end[0], b.end[1]);

    copyPlacesArray(sala.places, aux);

    markAsSelected(sala, start_p, end_p);

    printSala(sala); //este print no debería estar acá.
    // PREGUNTAR SI CONFIRMA
    printf("%s\n", "Ingrese una s si desea confirmar la selección o una n si desea modificarla");
    scanf("%c", &c);

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
	char * fname = get_fname (movie_name);
	sala_t sala = get_sala (fname);
	int* start;
	int* end;
	booking_t b;
	b.movie_name=movie_name;

	do {
        do {
            printSala(sala);
            start = getPosition("\nPlease choose the first position you want to buy\n");
            end = getPosition("\nPlease choose the last position you want to buy\n");
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
int action_buy_tickets() {
	char* cod = getMovieCode();
	booking_t booking = getBooking(cod);
	return buy_tickets(booking);
}
