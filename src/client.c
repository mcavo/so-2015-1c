/* actions client - front */

#include "../inc/client.h"
#include <ctype.h>

/* print sala with diferent colors (selected, bought, available) */
static void printSala (sala_t sala);

/* devuelve un puntero al titulo de la pelicula deseada */
/****** VER DE COPIAR A UN UNICO CHAR * Y LIBERAR LA MEM DE TODA LA MATRIZ *****/
static char* getMovieCode();

static void copyPlacesArray(int* first, int* second);

/* get positon of the a booking sit */
static void getPosition(int* pos, char * msg);

/* ask confirmation */
static BOOL askConfirmation(sala_t sala, booking_t b);

/* show sala and ask the sits to book */
static booking_t getBooking(char* movie_name);

static void printMovieList(fixture_t * fix);

/* Show the list of films */
int actionShowMovies() {
    
    printMovieList(get_movies());
    return 0;
}



//Retorna 0 si la operacion fue exitosa y -1 sino. En el front se trata la accion a seguir.
int actionBuyTickets() {
	char* cod = getMovieCode();
	booking_t booking = getBooking(cod);
	return buy_tickets(booking);
}

void static printSala (sala_t sala) {
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

/****** VER DE COPIAR A UN UNICO CHAR * Y LIBERAR LA MEM DE TODA LA MATRIZ *****/
static char* getMovieCode(){
	int index, i;
	fixture_t * fixture = get_movies();
    char * rta;
    printMovieList(fixture);
	do{
		printf("%s\n", "Seleccione el número de película que desea ver");
		scanf("%i", &index); 
	} while(!(0 < index && index <= fixture->count));
    for (i=0 ; i<fixture->count ; i++) {
        if(i != index -1)
            free(fixture->titles[i]);
    }
	rta =  fixture->titles[ index - 1];
    free(fixture->titles);
    free(fixture);
    return rta;
}

static void getPosition(int* pos, char * msg) {
    char cfil;
    int col;
    printf("%s\n", msg);
    scanf("%c%d",&cfil,&col);
    pos[0]=((int) (cfil-'a')) - 1;
    pos[1]=col - 1;
}

static void copyPlacesArray(int* first, int* second){
	int i;
    for (i = 0; i < MAX_PLACES; i++){
		*(second + i) = *(first + i);
	}
}

static BOOL askConfirmation(sala_t sala, booking_t b) {
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

static void printMovieList(fixture_t * fix) {
    if(fix==NULL) {
	printf("\n Database problem. Imposible to charge fixture.");
	return;
    }
    int i = 0;
    printf("\n  MOVIES LIST \n\
  -----------\n");
    
    for ( ; i < fix->count; i++) {
        printf("%i- %s\n", i + 1, fix->titles[i]);
    }
}
