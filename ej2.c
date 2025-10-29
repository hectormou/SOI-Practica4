#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/wait.h>

pid_t hijo;

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
        printf("El proceso hijo termina\n");
        exit(EXIT_SUCCESS);
    }

    pthread_exit(NULL);
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

    pthread_join(hilo1, NULL);
    pthread_join(hilo2, NULL);
    pthread_join(hilo3, NULL);
    
    exit(EXIT_SUCCESS);
}