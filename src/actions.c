#define STATUS_NUM          0
#define STATUS_TITLE        1

#define MAX_MOVIES          30

#define BLOCK           5

#include <stdio.h>

#include <db_connector.h>

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


booking_t getBooking(char* movie_name){
	/*bloquear para lectura y mostrarle la sala
	llamar a funcion en db_connector con movie_name que levante el archivo y desbloquee cuando decide.

*/
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
