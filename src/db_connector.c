#include "../inc/db_connector.h"

/** INCOMPLETA **/
static char * get_fname (char * movie_name);

static BOOL flock_creat(int start, int end, int fd, int type);

static BOOL flock_read(int start, int end, int fd);

static BOOL flock_write(int start, int end, int fd);

static BOOL flock_unlock(int start, int end, int fd);

/* charge titles into an array of strings */
static void charge_titles(fixture_t * fixture, int fd);

static void charge_sala(sala_t ** sala, int fd);

static void close_operation(int start, int end, int fd);

static BOOL seats_occupied(int start, int end, int fd);

static void get_seats(int start, int end, int fd);

static int open_file(char* movie_name);

sala_t * get_sala(char* pelicula) {
    sala_t * sala;
    char * fname = get_fname(pelicula);
    if (fname==NULL)
        return NULL;
    int fd = open(fname, O_RDWR);
    if (fd==-1) {
        return NULL;
    }
        
/* bloquear el archivo para la película dada */
    if(!flock_read(0, 0, fd)) { /* length 0, until EOF */
        close(fd);
        return NULL;
    }

/* cargar las ubicaciones disponibles para devolvérselas al cliente */
    charge_sala(&sala, fd);
    if (sala == NULL) {
        close(fd);
        return NULL;
    }
/* desbloquear la base */
    if(!flock_unlock(0, 0, fd)) {
        close(fd);
        return NULL;
    }
    close(fd);
/* retornar la sala */
    return sala;
}

fixture_t * get_movies() {
    int fd;
    fixture_t * ans;
    /* Abrir el archivo para lectura */

    if ((fd = open("../database/fixture.txt", O_RDONLY)) == -1) {
        return NULL;
    }
    if((ans = malloc(sizeof(fixture_t)))==NULL) {
        close(fd);
        return NULL;
    }
    /* Bloquear la base para lectura */
    if(!flock_read(0, 0, fd)) {
        close(fd);
        return NULL;
    }
    /* Charge movies */
    charge_titles(ans, fd);
    if(ans->titles ==NULL || !flock_unlock(0, 0, fd)) {
        close(fd);
        return NULL;
    }
    close(fd);
    return ans;
}


int buy_tickets(booking_t * booking){
    
    int start, end, fd;
    start = get_position(booking->start[0],booking->start[1]);//fila y columna paso
    end = get_position(booking->end[0],booking->end[1]);
    
    if(invalid_ticket_interval(start, end)||end>=MAX_PLACES||start<0)
        return INVALID_INTERVAL;
    fd = open_file(booking->movie_name);
    if (fd==-1)
        return DATABASE_ERROR;
    if(!flock_write(start, end, fd)) {
        close(fd);
        return DATABASE_ERROR;
    }
    if(seats_occupied(start,end,fd)){
        close_operation(start,end, fd);
        return OCCUPIED_SEATS;
    }
    
    get_seats(start, end, fd);
    close_operation(start,end, fd);

    return SUCCESFUL_OPERATION;
}

static char * get_fname (char * movie_name) {
    //FALTA EL RESTO DEL PATH!!!!
    char * path;
    char * p = "./database/";

    path = malloc((strlen(movie_name)+1)*sizeof(char));
    if(path==NULL)
        return path;
    memcpy(path,p,strlen(p)+1);

    int len = strlen(movie_name); //len: cant de letras del string sin incluir el '\0'
    char * fname = malloc((len+1)*sizeof(char));
    if(fname==NULL)
        return fname;

    int i=0;
    for(;movie_name[i]!='\0';i++) {
        if(movie_name[i]==' ')
            fname[i]='-';
        else
            fname[i]=tolower(movie_name[i]);
    }
    fname[i]='\0';
    fname = strcat(path,fname);
    fname = strcat(fname,".txt");
    return fname;
}

static BOOL flock_creat(int start, int end, int fd, int type) {
    struct flock fl;

    fl.l_type = type;
    fl.l_whence = SEEK_SET;
    fl.l_start = start;
    fl.l_len = end;
    fl.l_pid = getpid();

    if(fcntl(fd, F_SETLK, &fl) == -1) { //pensar bien los casos
        printf("Trying to connect to database. Wait please...\n");
        if (fcntl(fd, F_SETLKW, &fl) == -1) {
            return FALSE;
        }
    }
    return TRUE;
}

static BOOL flock_read(int start, int end, int fd) {
     return flock_creat(start, end, fd, F_RDLCK);
}

static BOOL flock_write(int start, int end, int fd) {
    return flock_creat(start, end, fd, F_WRLCK);
}

static BOOL flock_unlock(int start, int end, int fd) {
    return flock_creat(start, end, fd, F_UNLCK);
}

static void charge_titles(fixture_t * fixture, int fd){
    char c;
    int si=0, ci=0;
    if((fixture->titles = malloc(sizeof(char*)*MAX_SALA))==NULL)
        return;
	
    for(;si<MAX_SALA;si++) {
        fixture->titles[si] = malloc(sizeof(char)*(MAX_MOVIE_TITLE+1));
        if( fixture->titles[si] == NULL)
            break;
    }
    if(si<MAX_SALA) {
        for ( ci=0 ; ci < si ; ci++)
            free(fixture->titles[ci]);
        free(fixture->titles);
        fixture->titles=NULL;
        return;
    }
    si=0;
    while(read(fd,&c,1) != 0)
    {
        switch(c) {
            case '\n':
                (fixture->titles)[si][ci] = '\0';
                fixture->titles[si]=realloc(fixture->titles[si],sizeof(char)*(ci+1));
                si++;
                ci=0; //cambio de string
                break;
            default:
                if(si<MAX_SALA && ci<=MAX_MOVIE_TITLE) {
                    if(ci==MAX_MOVIE_TITLE)
                        (fixture->titles)[si][ci++] = '\0'; 
                    else
                        (fixture->titles)[si][ci++] = c; 
                }        
                break;
        }
    }
    fixture->titles=realloc(fixture->titles,sizeof(char*));
    (fixture->count)=si;
}

//VER DE HACERLA DEVOLVER UN INT POR SI EL ARCHIVO ESTA MALFORMADO
static void charge_sala(sala_t ** sala, int fd) {
    if ((*sala = malloc(sizeof(sala_t)))==NULL)
        return;

    int ri = 0, ci=0;
    char c;
    (*sala)->rows=MAX_ROW;
    (*sala)->cols=MAX_COL;
    while(read(fd,&c,1) != 0 && ((ri+1)*(ci+1) <= MAX_PLACES) ){ /* tendría que ser un get_int */
        (*sala)->places[ri][ci++] = c;
        if(ci%MAX_COL==0){
            ci=0;
            ri++;
        }
        
    }
}

static void close_operation(int start, int end, int fd){
    flock_unlock(start, end, fd);
    close(fd);
}

static BOOL seats_occupied(int start, int end, int fd){

    int i;
    char seat;

    for(i=start;i<=end;i++){

        lseek(fd, i, SEEK_SET);
        read(fd,&seat,1);
        
        if(seat==OCCUPIED)
            return TRUE;
    }

    return FALSE;
}

static void get_seats(int start, int end, int fd){

    int i;
    
    for(i=start;i<=end;i++){

        lseek(fd, i, SEEK_SET);
        write(fd,"1",1);

    }

}

static int open_file(char* movie_name){

    int fd;
    char* root = get_fname(movie_name);
    
    if ((fd = open(root , O_RDWR)) == -1) {
        perror("DB error.\n");
        return -1;
    }

    return fd;
} 