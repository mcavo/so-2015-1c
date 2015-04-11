#import "../inc/db_connector.h"



int buy_tickets(booking_t booking){
	int start = get_position(booking.start[0],booking.start[1]);//fila y columna paso
	int end = get_position(booking.end[0],booking.end[1]);
}

void flock_read(int start, int end, int fd) {
	flock_creat(start, end, fd, F_RDLCK);
}

void flock_write(int start, int end, int fd) {
	flock_creat(start, end, fd, F_WRLCK);
}

void flock_creat(int start, int end, int fd, int type) {
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

void flock_unlock(int start, int end, int fd) {
	flock_creat(start, end, fd, F_UNLCK);
}
