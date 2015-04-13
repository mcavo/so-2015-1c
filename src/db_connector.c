#import "../inc/db_connector.h"


//Retorna 0 si la operacion fue exitosa y -1 sino y -2 si el intervalo era invalido (ver si lo hacemos por errno). En el front se trata la accion a seguir.
int buy_tickets(booking_t booking){
	
    int start, end, fd;
    struct flock fl;

    start = get_position(booking.start[0],booking.start[1]);//fila y columna paso
	end = get_position(booking.end[0],booking.end[1]);
    
    if(invalid_ticket_interval(start, end))
        return INVALID_INTERVAL;

    fd = open_file(booking.movie_name);
    fl = flock_write(start, end, fd);

    if(seats_occupied(fd)){
        
        close_operation(fl, fd);
        return OCCUPIED_SEATS;
    }
    
    get_seats(start, end, fd);
    close_operation(fl, fd);


    return SUCCESFUL_OPERATION;


}

void close_operation(struct flock fl, int fd){
        flock_unlock(fl, fd);
        close(fd);
}

int seats_occupied(int start, int end, int fd){

    int i=start;
    char seat;

    for(i=start;i<=end;i++){

        lseek(fd, i, SEEK_SET);
        read(fd,&seat,1);
        
        if(seat==OCCUPIED)
            return 1;
    }

    return 0;
}

void get_seats(int start, int end, int fd){

    int i=start;
    
    for(i=start;i<=end;i++){

        lseek(fd, i, SEEK_SET);
        write(fd,"1",1);

    }

}

//Lo abrimos siempre en modo O_RDWR para que sea compatible con cualquier type de fcntl
int open_file(char* movie_name){

    int fd;
    char* root = get_file_root(movie_name);
    
    if ((fd = open(root , O_RDWR)) == -1) {
        perror("Error opening %s file",movie_name);
        return -1;
    }

    return fd;
} 

char* get_file_root(char* movie_name){
    char root [80]= "../database/"+ movie_name +".txt";
    strcpy(str, "../database/");
    strcat(str, movie_name);
    strcat(str, ".txt");
    
    return root;    
}

struct flock flock_read(int start, int end, int fd) {
	return flock_creat(start, end, fd, F_RDLCK);
}

struct flock flock_write(int start, int end, int fd) {
	return flock_creat(start, end, fd, F_WRLCK);
}

struct flock flock_creat(int start, int end, int fd, int type) {
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

    return fl;
}

void flock_unlock(struct flock fl, int fd) {
	fl.l_type = F_UNLCK;  /* set to unlock same region */

    if (fcntl(fd, F_SETLK, &fl) == -1) {
        perror("Cannot unlock");
    }
}
