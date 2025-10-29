/* Escribe un programa en el que se creen dos hilos. Cada uno de ellos debe dormir un tiempo
determinado e imprimir un mensaje por pantalla indicando el tiempo que ha dormido antes de
terminar. Utiliza pthread_join para garantizar que el segundo hilo imprime su mensaje siempre
después que el primero, aunque la duración del sleep del primero sea mayor que la del segundo*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>         

pthread_t hilo3;

void* h1(void* arg) {
    printf("Soy el hilo1 y voy a dormir 10 segundos\n");
    sleep(10);
    printf("Hilo1 terminado \n");
}
void* h2(void* arg) {
    pthread_t hilo1 = *(pthread_t*)arg;
    printf("Soy el hilo2 y voy a dormir 5 segundos\n");
    sleep(5);
    pthread_join(hilo1, NULL);
    printf("Hilo2 terminado \n");
}



int main() {
    pthread_t hilo1,hilo2;

    if (pthread_create(&hilo1, NULL, h1, NULL) != 0) {
        perror("Error al crear hilo1");
        return 1;
    }
    if (pthread_create(&hilo2, NULL, h2, &hilo1) != 0) {
        perror("Error al crear hilo2");
        return 1;
    }

    pthread_join(hilo2, NULL);
    return 0;
}