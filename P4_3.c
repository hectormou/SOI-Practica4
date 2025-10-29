/* Haz un programa con tres hilos (además del hilo principal) de modo que uno de ellos ejecute exit
en lugar de pthread_exit, antes de que los otros hilos terminen y comprueba que efecto tiene este
exit sobre los demás hilos. Debes garantizar que los hilos no finalizan previamente a la ejecución
de dicho exit.*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>         

pthread_t hilo3;

void* funcion_hilo(void* arg) {
    printf("Voy a esperar al hilo3 con pthread_join y luego imprimir un mensaje\n");
    if (pthread_join(hilo3, NULL)!= 0) {
        fprintf(stderr, "Error en pthread_join de hilo3\n");
    }else{
    printf("Hilo terminado \n");
    }
    return NULL;

}

void* funcion_exit(void* arg) {
    printf("Soy el Hilo 3.Voy a llamar a exit\n");
    exit(0);
    return NULL;
}


int main() {
    pthread_t hilo1,hilo2;

    if (pthread_create(&hilo3, NULL, funcion_exit, NULL) != 0) {
        perror("Error al crear hilo3");
        return 1;
    }

    if (pthread_create(&hilo1, NULL, funcion_hilo, NULL) != 0) {
        perror("Error al crear hilo1");
        return 1;
    }
    if (pthread_create(&hilo2, NULL, funcion_hilo, NULL) != 0) {
        perror("Error al crear hilo2");
        return 1;
    }

    pthread_join(hilo1, NULL);
    pthread_join(hilo2, NULL);
    pthread_join(hilo3, NULL);
    return 0;
}