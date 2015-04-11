#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define CLEAN_BUFF	while ( getchar() != '\n' );


int main() {
	struct flock fl = {F_WRLCK, SEEK_SET, 0, 0, 0 };
    int fd;
	int i=0;
	char c;

    fl.l_pid = getpid();

//LEVANTO ARCHIVO

    if ((fd = open("../database/malefica.txt", O_RDWR)) == -1) {
        perror("open");
        exit(1);
    }

//SETEO DONDE BLOQUEAR

    fl.l_start  = 0;        /* 0 = Offset from l_whence         */
	fl.l_len    = 0;        /* length, 0 = to EOF           */

//INTENTO BLOQUEAR
    printf("Press <RETURN> to try to get lock: \n");
    getchar();
    printf("Trying to get lock...\n");

	/*
	F_SETLK tira -1 cuando otro proceso esta bloqueando, lo usamos para avisar al usuario y llamar a
	flock otra vez con el modo F_SETLKW que espera automaticamente a que el otro proceso desbloquee.
	*/
    if(fcntl(fd, F_SETLK, &fl) == -1){
	printf("Trying to connect to database. Please wait...\n");
	if (fcntl(fd, F_SETLKW, &fl) == -1) {
        perror("fcntl");
        exit(1);

    	}

     }

    printf("got lock\n");

//LEER ESE ASIENTO

	//Se posiciona con lseek tantos bytes desde el comienzo del archivo.
        lseek(fd, 9, SEEK_SET);
	read(fd,&c,1);
	//c = fgetc(fd);
	if(c=='1'){
		printf("Los asientos estan ocupados. Intente nuevamente. \n");

		fl.l_type = F_UNLCK;  /* set to unlock same region */


	    if (fcntl(fd, F_SETLK, &fl) == -1) {
		perror("fcntl");
		exit(1);
	    }

	    printf("Unlocked.\n");
 		exit(1);

	}


//OCUPAR ASIENTO
    lseek(fd, 9, SEEK_SET);
    write(fd,"2",1);



    printf("Press <RETURN> to release lock: \n");
    CLEAN_BUFF
    getchar();

    fl.l_type = F_UNLCK;  /* set to unlock same region */

    if (fcntl(fd, F_SETLK, &fl) == -1) {
        perror("fcntl");
        exit(1);
    }

    printf("Unlocked.\n");

    close(fd);

    return 0;

}
