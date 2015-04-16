#include "../inc/db_connector.h"

/** INCOMPLETA **/
static char * get_fname (char * movie_name);

static void flock_creat(int start, int end, int fd, int type);

static void flock_read(int start, int end, int fd);

static void flock_write(int start, int end, int fd);

static void flock_unlock(int start, int end, int fd);

/** INCOMPLETA **/
static void write_booking(booking_t bogetoking, int fd);

/* charge titles into an array of strings */
static void charge_titles(fixture_t * fixture, int fd);

static void charge_sala(sala_t sala, int fd);

static BOOL db_valid_range( int* start, int* end, sala_t sala );

static BOOL check_valid_range(booking_t booking, int fd);

sala_t get_sala(char* pelicula) {
    sala_t sala;
    int fd = open(pelicula, O_RDWR);
    
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
    flock_unlock(0, 0, fd);
    ans = malloc(sizeof(fixture_t));
    /* Bloquear la base para lectura */
    flock_write(0, 0, fd);
    /* Charge movies */
    charge_titles(ans, fd);

    flock_unlock(0, 0, fd);
    close(fd);

    return ans;
}

void confirm_booking (booking_t booking) {
    int fd;
    fd = open(booking.movie_name, O_RDWR);

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

int buy_tickets(booking_t b){
	return 1;
}


static char * get_fname (char * movie_name) {
    //FALTA EL RESTO DEL PATH!!!!
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

static void write_booking(booking_t booking, int fd) {
    int count = booking.end - booking.start;
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

static void charge_sala(sala_t sala, int fd) {
    int index = 0;
    char c;
    int status = ROW_STATUS;
    while(read(fd,&c,1) != 0 && (status != END_STATUS)){ /* tendría que ser un get_int */
        switch(status) {
            case ROW_STATUS:
                sala.rows = sala.rows*10 + c - '0';
                if (c == ' ') 
                    status = COL_STATUS;
                break;
            case COL_STATUS:
                sala.cols = sala.cols*10 + c - '0';
                if (c == ' ') 
                    status = SITS_STATUS;
                break;
            case SITS_STATUS:
                if(index >= sala.rows * sala.cols) {
                    status = END_STATUS;
                    for( ; index < MAX_PLACES ; index ++){ 
                        sala.places[index] = -1;
                    }
                } else {
                    sala.places[index ++] = c;
                }
                break;
        }
    }
}

static BOOL db_valid_range( int* start, int* end, sala_t sala ) {
    int start_p = get_position(start[0], start[1]);
    int end_p = get_position(end[0], end[1]);
    int i;
    if(end_p >= start_p && end_p < MAX_PLACES) {
        for(i=0; i < (end_p - start_p); i++) {
            if (sala.places[start_p + i] == 1)
                return FALSE;
        }
        return TRUE;
    }
    return FALSE;
}

static BOOL check_valid_range(booking_t booking, int fd) {
    sala_t sala;
    charge_sala(sala, fd);
    return db_valid_range(booking.start, booking.end, sala);
}
