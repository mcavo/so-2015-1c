#define STATUS_NUM          0
#define STATUS_TITLE        1

#define MAX_MOVIES          30

#define TAKEN               '\e[0;31m' //RED
#define SELECTED            '\e[1;32m' //GREEN
#define FREE                ''


#include <stdio.h>
#include <string.h>
#include "db_connector.h"



//Va a ser por nombre de pelicula que coincide con el nombre del archivo.
static char* getMovieCode () {
    FILE* fd_movie;
    char c;
    int state = STATUS_NUM;
    int n = 1;
    // BLOQUEAR ESCRITURA
    fd_movie = fopen("fixture.txt","r");
    if(fd_movie==NULL) {
        printf("fallo fixture\n");
    return;
    }
    printf("\n  MOVIES LIST \n\
  -----------\n");
    while((c = fgetc(fd_movie))!=EOF)
    {
        switch(c) {
            case '\n':
                state=STATUS_NUM;
                break;
             default:
                if (state==STATUS_NUM) {
                    printf("\n  %d. ",n);
                    n++;
                    state=STATUS_TITLE;
                }
                printf("%c", c);
                break;
        }
    }
    printf("\n\n  Please, select your choice:\n");
    fclose(fd_movie);
    return NULL;
}

//Retorna 0 si la operacion fue exitosa y -1 sino. En el front se trata la accion a seguir.
int action_buy_tickets() {
	char* cod = getMovieCode();
	booking_t booking = getBooking(cod);
	return buy_tickets(booking);
}

static char * get_fname (char * movie_name) {
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

sala_t get_sala ( char * fname ) {
    int fd = open(fname,RDONLY)
    if (fd==-1)
        return NULL;
    flock_write(0,0,fd); //BLOQUEO LA ESCRITURA
    //el max de fil y col es de dos cifras, 2 espacios y maxfil*maxcol de 0s y 1s
    char * fsala = calloc(6+MAX_PLACES);  // ********MAGIC NUMBERRRRR: corregir despues
    if(fsala==NULL)
        return NULL;
    //LEER Y PONER EN FSALA
    flock_unlock(0,0,fd);
    int cur = 0;
    int rows=0,cols=0;
    while ( *(fsala+cur) != ' ' ) {
        rows = rows*10 + (*(fsala+cur)) - '0';
    }
    cur++;
    while ( *(fsala+i)!=' ' ) {
        cols = cols*10 + (*(fsala+(cur++)) - '0';
    }
    cur++;
    //Estoy en las fil y col
    int[MAX_PLACES] places;
    int i;
    for (i=0 ; i<MAX_PLACES ; i++,cur++) {
        if (i<rowx*cols)
            places[i]=*(fsala+cur)-'0';
        else
            places[i]=-1;
    }
    free(fsala);
    sala_t sala=malloc(sizeof(sala_t));
    sala.places=places;
    sala.rows=rows;
    sala.cols=cols;
    return sala;
}

int [2] getStart () {
    char cfil;
    int col;
    printf("\nPlease choose the first position you want to buy\n");
    scanf("%c%d",&cfil,&col);
    int [2] rta;
    rta[0]=((int) (cfil-'a')) - 1;
    rta[1]=col - 1;
}

int [2] getStart () {
    char cfil;
    int col;
    printf("\nPlease choose the first position you want to buy\n");
    scanf("%c%d",&cfil,&col);
    int [2] rta;
    rta[0]=((int) (cfil-'a')) - 1;
    rta[1]=col - 1;
}

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
            prinf("\n");
        printf("%cO",color);

    }
}

bool askConfirmation(sala_t sala,booking_t b,int[MAX_PLACES] newplaces) {
    int start_p = get_position(b.start[0],b.start[1]);
    int end_p = get_position(b.end[0],b.end[1]);
    int[MAX_PLACES] aux;
    for(i=0;i<sala.rows*sala.cols;i++) {
        if(i>=start_p&&i<=end_p) {
            newplaces[i]=2;
        } else {
          newplaces[i]=sala.places[i];
        }
    }
    for(;i<MAX_PLACES;i++) {
        newplaces[i]=-1;
    }
    aux = sala.places;
    sala.places = newplaces;
    printSala(sala);
    // PREGUNTAR SI CONFIRMA
}

bool validRange( int[2] start, int[2] end, sala_t sala ) {
    int start_p = get_position(start[0],start[1]);
    int end_p = get_position(end[0],end[1]);
    int i;
    if(end_p>=start_p && end_p<MAX_PLACES) {
        for(i=0;i<(end_p-start_p);i++) {
            if (sala.places[star_p+i]==1)
                return false;
        }
        return true;
    }
    return false;
}


booking_t getBooking(char* movie_name){
	/*bloquear para lectura y mostrarle la sala
	llamar a funcion en db_connector con movie_name que levante el archivo y desbloquee cuando decide.
	*/
	char * fname = get_fname (movie_name);
	sala_t sala = get_sala (fname);
	int[2] start,end;
	int[MAX_PLACES] newplaces;
	booking_t b = malloc(sizeof(booking_t));
	b.name=movie_name;
	do {
        do {
            printSala(sala);
            start = getStart();
            end = getEnd();
        } while ( !validRange(start,end,sala) );
        b.start=start;
        b.end=end;

	} while ( askConfirmation(sala,b,newplaces) );


}


void action_show_movies() {
    FILE* fd_movie;
    char c;
    int state = STATUS_NUM;
    int n = 1;
    // BLOQUEAR ESCRITURA
    fd_movie = fopen("fixture.txt","r");
    if(fd_movie==NULL) {
        printf("fallo fixture\n");
        return;
    }
    printf("\n  MOVIES LIST \n\
  -----------\n");
    while((c = fgetc(fd_movie))!=EOF)
    {
        switch(c) {
            case '\n':
                state=STATUS_NUM;
                break;
            default:
                if (state==STATUS_NUM) {
                    printf("\n  %d. ",n);
                    n++;
                    state=STATUS_TITLE;
                }
                printf("%c", c);
                break;
        }
    }
    printf("\n\n  Please, select your choice:\n");
    fclose(fd_movie);
    return;
}
