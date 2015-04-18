/* actions client - front */

#include "../inc/client.h"
#include <ctype.h>
  

/* print sala with diferent colors (selected, bought, available) */
static void printSala (sala_t sala);

/* devuelve un puntero al titulo de la pelicula deseada */
/****** VER DE COPIAR A UN UNICO CHAR * Y LIBERAR LA MEM DE TODA LA MATRIZ *****/
static char* getMovieTitle();

/* get positon of the a booking sit */
static void getPosition(int pos[2], char * msg);

/* ask confirmation */
static BOOL askConfirmation(sala_t * sala, booking_t * b);

/* show sala and ask the sits to book */
static booking_t* getBooking(char* movie_name);

static void printMovieList(fixture_t * fix);

/* Show the list of films */
int actionShowMovies() {
    printMovieList(get_movies());
    return 0;
}

//Retorna 0 si la operacion fue exitosa y -1 sino. En el front se trata la accion a seguir.
int actionBuyTickets() {
	char* title = getMovieTitle();
	booking_t * booking = getBooking(title);
    if(booking!=NULL)
	   return buy_tickets(booking);
    return -1;
}

void static printSala (sala_t sala) {
    int ri,ci;
    char row='A';
    char* color;
    printf("\t");
    for(ci=0;ci<MAX_COL;ci++) {
        printf("%d ",ci+1);
    }
    printf("\n");
    for(ri=0;ri<MAX_ROW;ri++) {
        printf("\n  %c\t",row+ri);
        for(ci=0;ci<MAX_COL;ci++) {
            if(sala.places[ri][ci]=='1')
                color=ANSI_COLOR_BOUGHT;
            else {
                if(sala.places[ri][ci]=='2')
                    color=ANSI_COLOR_SELECTED;
                else
                    color=ANSI_COLOR_AVAILABLE;
            }

            printf("%sO%s ",color,ANSI_COLOR_RESET);
        }
    }
    printf("\n\n");
}

static char* getMovieTitle(){
	int index, i;
	fixture_t * fixture = get_movies();
    char * rta;
    printMovieList(fixture);
	do{
		printf("%s\n", "Seleccione el número de película que desea ver");
		scanf("%d", &index); 
        fflush( stdin );
	} while(!(0 < index && index <= fixture->count));
    rta =  fixture->titles[ index - 1];
    printf("%s\n",rta);
    for (i=0 ; i<fixture->count ; i++) {
        if(i != index -1)
            free(fixture->titles[i]);
    }
    free(fixture->titles);
    free(fixture);
    printf("You chose: %s\n",rta);
    return rta;
}

static void getPosition(int pos[2], char * msg) {
    char crow;
    int col;
    printf("%s\n", msg);
    scanf("%s %d",&crow,&col);
    pos[0]=(int) (tolower(crow)-'a');
    pos[1]=col - 1;
}

static int askConfirmation(sala_t * sala, booking_t * b) {
    char c;
    char aux[MAX_ROW][MAX_COL];

    int start_p = get_position(b->start[0], b->start[1]);
    int end_p = get_position(b->end[0], b->end[1]);

    memcpy(aux,sala->places,MAX_PLACES);

    markAsSelected(start_p,end_p,sala);

    printSala(*sala); //este print no debería estar acá.
    // PREGUNTAR SI CONFIRMA
    //SPANGLISH ALERT! 
    printf("%s\n", "Desea confirmar su reserva? (S/N)");
    do {
        printf("%s\n","Ingrese Q para cancelar la transaccion.");
        fflush( stdin );
        scanf("%s", &c);
    } while(tolower(c)!='n'&&tolower(c)!='s'&& tolower(c)!='q'&&printf("Por favor ingrese una S o una N.\n")); 
    switch(tolower(c)){
    	case 's':
    		return CONFIRM;
    	case 'n':
    		memcpy(sala->places,aux,MAX_PLACES);
    		return DONT_CONFIRM;
        default:
            return QUIT;
    }; //hay que ver si esto está realmente bien. Si ingresa otra cosa
}

booking_t * getBooking(char* movie_name) {
	sala_t* sala = get_sala (movie_name);
	int start[2];
	int end[2];
	booking_t * b = calloc(1,sizeof(booking_t));
	b->movie_name=movie_name;
    int aux;
	do {
        do {
            printSala(*sala);
            //fflush( stdin );
            getPosition(start,"\nPlease choose the first position you want to buy\n");
            fflush( stdin );
            getPosition(end,"\nPlease choose the last position you want to buy\n");
            fflush( stdin );
        } while ( !validRange(start, end, sala) ); //habría que ver si podemos dar un poco más de feedback para saber caul es el problema
        memcpy(b->start,start,2*sizeof(int));
        memcpy(b->end,end,2*sizeof(int));
	} while ( (aux=askConfirmation(sala, b))==DONT_CONFIRM );
    if(aux==QUIT)
        b=NULL;
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
