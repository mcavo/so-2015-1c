/* actions client - front */

#include "../inc/client.h"
#include <ctype.h>

#define clean while(getchar()!='\n')
  

/* print sala with diferent colors (selected, bought, available) */
static void printSala (sala_t sala);

/* devuelve un puntero al titulo de la pelicula deseada */
/****** VER DE COPIAR A UN UNICO CHAR * Y LIBERAR LA MEM DE TODA LA MATRIZ *****/
static char* getMovieTitle(int * code);

/* get positon of the a booking sit */
static void getPosition(int pos[2], char * msg);

/* ask confirmation */
static BOOL askConfirmation(sala_t * sala, booking_t * b);

/* show sala and ask the sits to book */
static booking_t* getBooking(char* movie_name, int * code);

static void printMovieList(fixture_t * fix);

/* Show the list of films */
void actionShowMovies() {
    fixture_t * f = get_movies();
    if (f==NULL)
        printMsg(DATABASE_ERROR);
    else
        printMovieList(f);
    return;
}

//Retorna 0 si la operacion fue exitosa y -1 sino. En el front se trata la accion a seguir.
void actionBuyTickets() {
    int code;
	char* title = getMovieTitle(&code);
    if (title == NULL) {
        printMsg(code);
        return;
    }
    do {
	    booking_t * booking = getBooking(title,&code);
        if(booking==NULL) {
            printMsg(code);
            return;
        }
        printMsg(buy_tickets(booking));
    } while (code==INVALID_INTERVAL || code==OCCUPIED_SEATS);

    return;
}

void printMsg ( int code ) {
    char * ERROR_ANSI_COLOR = "\e[1;31m";
    char * SUCCESFULL_ANSI_COLOR = "\e[1;34m";
    char * RESET_ANSI_COLOR = "\e[1;0m";
    switch (code) {
        case INVALID_INTERVAL:
            printf("%s  Error: Invalid interval.%s\n",ERROR_ANSI_COLOR,RESET_ANSI_COLOR);
            break;
        case OCCUPIED_SEATS:
            printf("%s  Error: Occupied seats.%s\n",ERROR_ANSI_COLOR,RESET_ANSI_COLOR);
            break;
        case DATABASE_ERROR:
            printf("%s  Error: Database general error.%s\n",ERROR_ANSI_COLOR,RESET_ANSI_COLOR);
            break;
        case CANCELLED_OPERATION:
            printf("%s  Cancelled operation.%s\n",SUCCESFULL_ANSI_COLOR,RESET_ANSI_COLOR);
            break;
        case OUT_OF_MEMORY:
            printf("%s  Error: Out of memory.%s\n",ERROR_ANSI_COLOR,RESET_ANSI_COLOR);
            break;
        case SUCCESFUL_OPERATION:
            printf("%s  Succesful operation!%s\n",SUCCESFULL_ANSI_COLOR,RESET_ANSI_COLOR);
            break;
        default:
            break;
    }
}

void static printSala (sala_t sala) {
    int ri,ci;
    char row='A';
    char* color;
    printf("\n\t");
    for(ci=0;ci< (2*MAX_COL-7)/2 ; ci++) {
        printf("%c",' ');
    }
    printf("%s\n\t","screen");
    for(ci=0;ci<(2*MAX_COL-1);ci++){
        printf("%c",'-');
    }
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
    printf("\n\n\t");
    for(ci=0;ci<MAX_COL;ci++) {
        printf("%d ",ci+1);
    }
    printf("\n");
}

static char* getMovieTitle(int * code){
	int index, i;
	fixture_t * fixture = get_movies();
    if (fixture==NULL) {
        *code = DATABASE_ERROR;
        return NULL;
    }
    char * rta;
    printMovieList(fixture);
	do{
		printf("  %s\n   ", "Please, select your choise:");
		scanf("%d", &index); 
        clean;
	} while(!(0 < index && index <= fixture->count));
    rta =  fixture->titles[ index - 1];
    for (i=0 ; i<fixture->count ; i++) {
        if(i != index -1)
            free(fixture->titles[i]);
    }
    free(fixture->titles);
    free(fixture);
    printf("\n  You chose: %s!\n",rta);
    return rta;
}

static void getPosition(int pos[2], char * msg) {
    char crow;
    int col;
    printf("\n  %s\n   ", msg);
    scanf("%c %d",&crow,&col);
    clean;
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

    printSala(*sala); 
    printf("%s\n", "  Do you want to continue? (Y/N)");
    do {
        printf("%s\n   ","  Insert Q to quit.");
        scanf("%c", &c);
        clean;
        printf("\n");
    } while(tolower(c)!='n'&&tolower(c)!='y'&& tolower(c)!='q'&&printf("Please insert Y,N or Q.\n")); 
    switch(tolower(c)){
    	case 'y':
    		return CONFIRM;
    	case 'n':
    		memcpy(sala->places,aux,MAX_PLACES);
    		return DONT_CONFIRM;
        default:
            return QUIT;
}

booking_t * getBooking(char* movie_name, int * code) {
	sala_t* sala = get_sala (movie_name);
    if (sala==NULL) {
        *code = DATABASE_ERROR;
        return NULL;
    }
	int start[2];
	int end[2];
	booking_t * b = calloc(1,sizeof(booking_t));
    if (b== NULL) {
        *code = OUT_OF_MEMORY;
        return NULL;
    }
	b->movie_name=movie_name;
    int aux;
	do {
        do {
            printSala(*sala);
            do {
                getPosition(start,"Please choose the first position you want to buy. [ROW COL]");
            } while(!validPosition(start));
            do {
                getPosition(end,"Please choose the last position you want to buy. [ROW COL]");
                
            } while(!validPosition(end));
        } while ( !validRange(start, end, sala)); //habría que ver si podemos dar un poco más de feedback para saber caul es el problema
        memcpy(b->start,start,2*sizeof(int));
        memcpy(b->end,end,2*sizeof(int));
	} while ( (aux=askConfirmation(sala, b))==DONT_CONFIRM );
    if(aux==QUIT) {
        free(b);
        b=NULL;
        *code = CANCELLED_OPERATION;
    }
	return b;
}

static void printMovieList(fixture_t * fix) {
    if(fix!=NULL) {
        int i = 0;
        printf("\n  Fixture \n");
        printf("  --------\n");
    
        for ( ; i < fix->count; i++) {
            printf("  %i.%s\n", i + 1, fix->titles[i]);
        }
        printf("\n");
    }
}
