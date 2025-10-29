#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/wait.h>


pid_t hijo;

/* se comprueba con ps u -L o con ps -L -p <PID> que efectivamente el hijo sólo se crea con un sólo hilo*/
void *rutinafork(void *arg) {

    printf("El hilo 3 en ejecucion y listo para hacer el fork\n");
    hijo=fork();
    if(hijo<0) {
        perror("Error en el fork\n");
        pthread_exit(NULL);         /*salimos del hilo, no del proceso*/
    } else if(hijo==0) {
        /*proceso hijo*/
        printf("Soy el proceso hijo creado por el hilo 3, comprueba cuantos hilos tengo\n");
        printf("PID:\n");
        printf("Hijo:\t%d\t\tPadre:\t%d\n", getpid(), getppid());
        pause();
        printf("El proceso hijo termina\n");
        exit(EXIT_SUCCESS);
    }

    sleep(10);              /*para que de tiempo a ver que existe*/

    pthread_exit(NULL);     /*cuando se muere el hilo al hijo nacido a partir de él no le afecta*/
}

void *esperarthread(void *arg) {
    printf("Soy el hilo cualquiera y me duermo...\n");
    sleep(10);          /*garantizamos que estos hilos no han acabado cuando hilo3 ejecuta el fork*/

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    pthread_t hilo1, hilo2, hilo3;

    pthread_create(&hilo1, NULL, esperarthread, NULL);
    pthread_create(&hilo2, NULL, esperarthread, NULL);
    pthread_create(&hilo3, NULL, rutinafork, NULL);

    //pause();                    /*introducimos esto para ver en qué estado se quedan los hilos pasados 10 segundos*/
                                /*parece que los hilos no quedan en estado zombie, directamente se eliminan*/

    /*si se crea un hijo aqui, ¿cuantos hilos tiene?*/
    pid_t hijo2=fork();
    if(hijo2 < 0) {
        perror("Error en el fork\n");
        exit(EXIT_FAILURE);
    } else if(hijo2==0) {
        printf("Soy el hijo 2 con PID: %d\n", getpid());
        /* \n importantisimo para que stdout haga flush automatico en terminal*/
        /*si no se hace pasa que el buffer se sobreescriba y no se viese el mensaje*/
        pause();                /*para comprobar cuantos hilos tiene*/
    }
    /*solo se forkea un hilo, en este caso el hilo main*/

    /*dato curioso, al interrumpir el proceso padre con CTRL + C se terminan tambien los procesos hijos*/
    /*SIG_INT se envia a todo el grupo de procesos (padre + hijos)*/

    pthread_join(hilo1, NULL);
    pthread_join(hilo2, NULL);
    pthread_join(hilo3, NULL);

    wait(NULL);
    
    exit(EXIT_SUCCESS);
}