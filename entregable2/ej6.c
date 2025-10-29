/*
    * Authors: Héctor Moure Rodríguez y Jorge García Lado
*/

//gcc ej6.c -Wall -lpthread -lm

/*
    * Esta máquina puede soportar hasta 2 hilos por core, de modo que el número máximo de hilos en paralelo de forma teórica en 8,
    * utilizar 8 sin duda mejora la velocidad respecto de la prueba con 4 hilos. A partir de aqui el aumento del número de hilos no
    * parece significativo pues las pruebas indican que la mejora es mínima o ni si quiera perceptible para el usuario pues el sistema
    * operativo puede no estar priorizando la ejecución de este programa, sin embargo, el uso de 1000 hilos sí parece tener una mejora
    * notoria en el rendimiento del programa, se han repetido las pruebas para varios valores distintos de iter y siempre proporciona
    * el mejor resultado, por lo que parece un punto óptimo. Superar los 1000 hilos produce el efecto contrario, el programa se vuelve
    * más lento pues la acción de ceder la cpu a otro hilo se vuelve demasiado frecuente y provoca más una sobrecarga computacional 
    * que un aumento en la velocidad.
    * 
    * 
    * Tiempo con 4 hilos:       Tiempo (ejecución paralela):          14.093685 segundos
         (1 hilo por core)      Tiempo (ejecución secuencial):        30.808217 segundos
    * Tiempo con 8 hilos:       Tiempo (ejecución paralela):          13.976246 segundos
                                Tiempo (ejecución secuencial):        28.255944 segundos
    * Tiempo con 16 hilos:      Tiempo (ejecución paralela):          12.024400 segundos
                                Tiempo (ejecución secuencial):        33.810877 segundos
    * Tiempo con 32 hilos:      Tiempo (ejecución paralela):          13.532166 segundos
                                Tiempo (ejecución secuencial):        32.971455 segundos
    * Tiempo con 64 hilos:      Tiempo (ejecución paralela):          13.295196 segundos
                                Tiempo (ejecución secuencial):        33.431646 segundos
    * Tiempo con 1000 hilos:    Tiempo (ejecución paralela):          11.273955 segundos
                                Tiempo (ejecución secuencial):        25.514843 segundos
    * Tiempo con 10000 hilos:   Tiempo (ejecución paralela):          13.643041 segundos
                                Tiempo (ejecución secuencial):        30.841194 segundos
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h>
#include <sys/time.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define H 8                 /*4 cores con 2 hilos por core*/
#define iter 1000000000

int modulo=iter/H;
int resto= iter%H;

void *calculohilo(void *arg) {
    int id=*(int*)arg;
    int n=modulo*id+1, hasta=n + modulo;
    double *sumalocal=(double*)malloc(sizeof(double));

    *sumalocal=0;
    for( ; n<hasta; n++) {
        *sumalocal += 1/pow((double)n,(double)2);
        //*sumalocal=*sumalocal + 1;
    }
    //printf("EL hilo con id: %d ha calculado: %f\n", id,*sumalocal);

    if(id==0) {                                 /*caso especial para que el primer hilo calcule tambien el resto*/
        if(resto!=0) {
            n=modulo*H;
            hasta=n + resto;
            for( ; n<hasta; n++) {
                *sumalocal += 1/pow((double)n,(double)2);
                //*sumalocal=*sumalocal + 1;
                //printf("EL hilo con id: %d ha calculado: %f\n", id,*sumalocal);
            }
        }
    }

    pthread_exit((void*)sumalocal);
}

int main (int argc, char *argv[]) {
    int ids[H];
    pthread_t hilos[H];
    double sumasparalelas=0.0, sumaserie=0.0;
    struct timeval inicio, fin;
    double tiempohilos, tiemposerie;
 
    gettimeofday(&inicio, NULL);
    for(int i=0; i<H; i++) {                    /*bucle para asignar id*/   /*bucle para crear hilos*/
        ids[i]=i;
        if(pthread_create(&hilos[i], NULL, calculohilo, &ids[i])!=0) {
            printf("No se pudo crear el hilo: %d\n", ids[i]);
            exit(EXIT_FAILURE);
        } else printf("Se ha creado el hilo: %d\n", ids[i]);
    }

    for(int i=0; i<H; i++) {                    /*bucle para esperar a que acaben todos los hilos y recuperar las sumas locales*/
        double *resultado_hilo;
        pthread_join(hilos[i], (void **)&resultado_hilo);
        sumasparalelas += *resultado_hilo;      /*copiar el valor real*/
        free(resultado_hilo);                   /*liberar memoria asignada por el hilo*/
    }
    gettimeofday(&fin, NULL);

    tiempohilos = (fin.tv_sec - inicio.tv_sec) + (fin.tv_usec - inicio.tv_usec) / 1000000.0;
    
    gettimeofday(&inicio, NULL);
    for(int n=1; n<=iter; n++) {                /*calculo unicamente en el hilo principal*/
        sumaserie += 1/pow((double)n,(double)2);
        //sumaserie=sumaserie + 1;
    }
    gettimeofday(&fin, NULL);
    tiemposerie = (fin.tv_sec - inicio.tv_sec) + (fin.tv_usec - inicio.tv_usec) / 1000000.0;

    /*salida de informacion*/
    printf("\n==============================================\n");
    printf("        RESULTADOS DEL CÁLCULO DE LA SERIE\n");
    printf("==============================================\n");
    printf("Serie (suma paralela con hilos):      %.17f\n", sumasparalelas);
    printf("Serie (suma secuencial en main):      %.17f\n", sumaserie);
    printf("----------------------------------------------\n");
    printf("Valor teórico (pi² / 6):              %.17f\n", (M_PI * M_PI / 6.0));
    printf("----------------------------------------------\n");
    printf("Diferencia entre ambas sumas:         %.17f\n", fabs(sumasparalelas - sumaserie));
    printf("Diferencia (paralela vs pi²/6):       %.17f\n", fabs(sumasparalelas - (M_PI * M_PI / 6.0)));
    printf("Diferencia (secuencial vs pi²/6):     %.17f\n", fabs(sumaserie - (M_PI * M_PI / 6.0)));
    printf("----------------------------------------------\n");
    printf("Tiempo (ejecución paralela):          %.6f segundos\n", tiempohilos);
    printf("Tiempo (ejecución secuencial):        %.6f segundos\n", tiemposerie);
    printf("Ahorro en tiempo usando hilos:        %.6f segundos\n", tiemposerie-tiempohilos);
    printf("==============================================\n\n");

    exit(EXIT_SUCCESS);
}