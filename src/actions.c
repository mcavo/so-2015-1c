#define STATUS_NUM          0
#define STATUS_TITLE        1

#define MAX_MOVIE_LENGHT    50
#define MAX_MOVIES          30

#define BLOCK           5

#include <stdio.h>

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

void action_show_movies() {
    char* cod = getMovieCode();
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