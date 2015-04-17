#include "../inc/db_connector.h"

/** INCOMPLETA **/
static char * get_fname (char * movie_name);

static void flock_creat(int start, int end, int fd, int type);

static void flock_read(int start, int end, int fd);

static void flock_write(int start, int end, int fd);

static void flock_unlock(int start, int end, int fd);

/** INCOMPLETA **/
static void write_booking(booking_t * bogetoking, int fd);

/* charge titles into an array of strings */
static void charge_titles(fixture_t * fixture, int fd);

static void charge_sala(sala_t * sala, int fd);

static BOOL db_valid_range( int* start, int* end, sala_t * sala );

static BOOL check_valid_range(booking_t * booking, int fd);

static void close_operation(int start, int end, int fd);

static BOOL seats_occupied(int start, int end, int fd);

static void get_seats(int start, int end, int fd);

static int open_file(char* movie_name);

sala_t * get_sala(char* pelicula) {
    sala_t * sala = malloc(sizeof(sala_t));
    char * fname = get_fname(pelicula);
    int fd = open(fname, O_RDWR);
    if (fd==-1) {
        perror("open fixture");
        exit(1);
    }
        
/* bloquear el archivo para la película dada */
    flock_read(0, 0, fd); /* length 0, until EOF */

/* cargar las ubicaciones disponibles para devolvérselas al cliente */
    charge_sala(sala, fd);

/* desbloquear la base */
    flock_unlock(0, 0, fd);
    close(fd);
/* retornar la sala */
    return sala;
}

fixture_t * get_movies() {
    int fd;
    fixture_t * ans;
    /* Abrir el archivo para lectura */

    if ((fd = open("./database/fixture.txt", O_RDONLY)) == -1) {
        perror("open fixture");
        return NULL;
    }
    ans = malloc(sizeof(fixture_t));
    /* Bloquear la base para lectura */
    flock_read(0, 0, fd);
    /* Charge movies */
    charge_titles(ans, fd);

    flock_unlock(0, 0, fd);
    close(fd);

    return ans;
}

void confirm_booking (booking_t * booking) {
    int fd;
    fd = open(booking->movie_name, O_RDWR);

/* Bloquear la base para que no puedan leer ni escribir y abrir la conexión */
    flock_write(0, 0, fd);

/* Checkear que los asientos estén disponibles */
    if(check_valid_range(booking, fd)){
    
    /* Modificar los asientosa ocupados */
        write_booking(booking, fd);
    }
/* Liberar la base */
    flock_unlock(0, 0, fd);
    close(fd);
}


int buy_tickets(booking_t * booking){
    
    int start, end, fd;
    sala_t * sala = get_sala(booking->movie_name);
    start = get_position(booking->start[0],booking->start[1],sala->cols);//fila y columna paso
    end = get_position(booking->end[0],booking->end[1],sala->cols);
    
    if(invalid_ticket_interval(start, end)||end>=(sala->cols*sala->rows)||start<0)
        return INVALID_INTERVAL;
    start+=CHAR_LEN_ROW+CHAR_LEN_COL+2;
    end+=CHAR_LEN_ROW+CHAR_LEN_COL+2;
    fd = open_file(booking->movie_name);
    flock_write(start, end, fd);

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
    memcpy(path,p,strlen(p));
    int len = strlen(movie_name); //len: cant de letras del string sin incluir el '\0'
    char * fname = malloc((len+1)*sizeof(char));
    int i=0;
    for(;movie_name[i]!='\0';i++) {
        if(movie_name[i]==' ')
            fname[i]='_';
        else
            fname[i]=tolower(movie_name[i]);
    }
    fname[i]='\0';
    fname = strcat(path,fname);
    fname = strcat(fname,".txt");
    //free(path); tiene problemas ??????
    return fname;
}

static void flock_creat(int start, int end, int fd, int type) {
    struct flock fl;

    fl.l_type = type;
    fl.l_whence = SEEK_SET;
    fl.l_start = start;
    fl.l_len = end;
    fl.l_pid = getpid();

    if(fcntl(fd, F_SETLK, &fl) == -1) { //pensar bien los casos
        printf("Trying to connect to database. Wait please...\n");
        if (fcntl(fd, F_SETLKW, &fl) == -1) {
          perror("fcntl");
          exit(1);
        }
    }

    printf("got lock\n"); //DEBUG
}

static void flock_read(int start, int end, int fd) {
    flock_creat(start, end, fd, F_RDLCK);
}

static void flock_write(int start, int end, int fd) {
    flock_creat(start, end, fd, F_WRLCK);
}

static void flock_unlock(int start, int end, int fd) {
    flock_creat(start, end, fd, F_UNLCK);
}

static void write_booking(booking_t * booking, int fd) {
    int count = booking->end - booking->start;
    char* data = calloc(count,sizeof(char));
    int i;
    for(i = 0; i < count; i++){
        data[i] = '1';
    }
    /* queda ver como se escribe en el archivo */
    //fwrite(fd, count, );
}

static void charge_titles(fixture_t * fixture, int fd){
    char c;
    int si=0, ci=0;
    fixture->titles = malloc(sizeof(char*)*MAX_SALA);
	//perror("Not enough memory");
    for(;si<MAX_SALA;si++) {
	   fixture->titles[si] = malloc(sizeof(char)*(MAX_MOVIE_TITLE+1));
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
static void charge_sala(sala_t * sala, int fd) {
    int index = 0;
    char c;
    int status = ROW_STATUS;
    sala->rows=0;
    sala->cols=0;
    while(read(fd,&c,1) != 0 && (status != END_STATUS)){ /* tendría que ser un get_int */
        switch(status) {
            case ROW_STATUS:
                if (c == ' ') 
                    status = COL_STATUS;
                else
                    sala->rows = sala->rows*10 + c - '0';
                break;
            case COL_STATUS:
                if (c == ' ') {
                    status = SITS_STATUS;
                    sala->places=malloc(((sala->rows*sala->cols) +1)*sizeof(char));
                } else
                    sala->cols = sala->cols*10 + c - '0';
                break;
            case SITS_STATUS:
                if(index < sala->rows * sala->cols) {
                    sala->places[index ++] = c;
                }
                break;
        }
    }
    sala->places[sala->rows*sala->cols]='\0';
}

static BOOL db_valid_range( int* start, int* end, sala_t * sala ) {
    int start_p = get_position(start[0], start[1],sala->cols);
    int end_p = get_position(end[0], end[1],sala->cols);
    int i;
    if(end_p >= start_p && end_p < sala->cols*sala->rows) {
        for(i=0; i < (end_p - start_p); i++) {
            if (sala->places[start_p + i] == 1)
                return FALSE;
        }
        return TRUE;
    }
    return FALSE;
}

static BOOL check_valid_range(booking_t * booking, int fd) {
    sala_t * sala;
    charge_sala(sala, fd);
    return db_valid_range(booking->start, booking->end, sala);
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