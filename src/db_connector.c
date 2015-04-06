#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#define CLEAN_BUFF	while ( getchar() != '\n' );


int main() {
	struct flock fl = {F_WRLCK, SEEK_SET, 0, 0, 0 };
    int fd;
    printf("%d\n",sizeof(char));
    fl.l_pid = getpid();

    if ((fd = open("../database/malefica.txt", O_WRONLY)) == -1) {
        perror("open");
        exit(1);
    }

    fl.l_start  = 4;        /* Offset from l_whence         */
	fl.l_len    = 2;        /* length, 0 = to EOF           */
    printf("Press <RETURN> to try to get lock: ");
    getchar();
    printf("Trying to get lock...");

    if (fcntl(fd, F_SETLKW, &fl) == -1) {
        perror("fcntl");
        exit(1);
    }

    printf("got lock\n");

    write(fd,"la",2);

    printf("Press <RETURN> to release lock: ");
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