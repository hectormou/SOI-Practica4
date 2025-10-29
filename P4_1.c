/*Haz un programa para comprobar que tanto las variables globales como las locales a los hilos son
accesibles por todos ellos. Comprueba que si un hilo cambia una de esas variables, los demás ven
dicho cambio*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>         

int variable_global= 5;


void* funcion_hilo(void* arg) {
    int num = *((int*)arg);
    printf("El hilo recibe la variable local con valor: %d\n", num);
        printf("El hilo %d está sumando 5 a las variables globales y locales\n", num);
        variable_global += 5;
        num += 5;

        printf("Variable global modificada: %d\n", variable_global);
        printf("Variable local modificada: %d\n", num);
}

int main() {
    int variable_local = 10;
    pthread_t hilo;
    printf("Valor de la variable local en main antes de crear el hilo: %d\n", variable_local);
    printf("Valor de la variable global en main antes de crear el hilo: %d\n", variable_global);
    pthread_create(&hilo, NULL, funcion_hilo,&variable_local);
    pthread_join(hilo, NULL);

    printf("Valor de la variable local en main después de ejecutar el hilo: %d\n", variable_local);
    printf("Valor de la variable global en main después de ejecutar el hilo: %d\n", variable_global);
    return 0;
}