#ifndef __SHMPOSIX_H__
#define __SHMPOSIX_H__

#define SIZE 1000

/* Numeros de semaforo */
#define CLIENT_WRITE 1
#define SERVER_READ	 2
#define CLIENT_READ  3
#define SERVER_WRITE 4

typedef enum { false, true } bool;

/* Fatal:                                            */
/* Descripcion:   Imprime mensaje de error y termina */
/*                la ejecucion.                      */
/* @param s:      String que se imprimira como       */
/*                mensaje de error.                  */
void fatal(char *s);

/* Init_mutex:                                       */
/* Descripcion:   Inicializa los semaforos.          */
void init_mutex(void);

/* Close_mutex:                                      */
/* Descripcion:   Cierra los semaforos.              */
/* @return:       Devuelve EXIT_SUCCESS o            */
/*                EXIT_FAILURE.                      */
int close_mutex(void);

/* Getmem:                                           */
/* Descripcion:   Inicializa la memoria compartida   */
/*                de un tamaño SIZE.                 */
/* @return:       puntero al comienzo de la zona de  */
/*                shm.                               */
/*********VER QUE PARAMETROS CONVIENE RECIBIR*********/                                 
char *getmem(void);

/* Freemem:                                          */
/* Descripcion:   Libera la memoria compartida.      */
void freemem();

/* Handle_signal:                                    */
void handle_signal(int signal);

/* Install_signal_handler:                           */
void install_signal_handler();

/* Enter:                                            */
/* Descripcion: up sobre el semaforo.                */
/* @param numsem: numero de semaforo.                */
void enter(int numsem);

/* Leave:
/* Descripcion: down sobre el semaforo.              */
/* @param numsem: numero de semaforo.                */
void leave(int numsem);

#endif