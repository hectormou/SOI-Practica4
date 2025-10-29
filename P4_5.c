/*.ENTREGABLE 1 Haz un programa para comprobar el efecto de la función sched_yield
(pthread_yield puede usarse en versiones antiguas de la biblioteca de C de GNU). Una forma
sencilla de hacer esto es crear un programa con varios hilos, de modo que unos sean generosos y
otros competitivos al liberar la CPU.
Para ello, implementa un programa que cree dos grupos de hilos que compitan por realizar un
cálculo intensivo de CPU: la aproximación del número π mediante el método de Montecarlo.
• Cada hilo debe realizar el mismo trabajo de cálculo: generar (1e9) pares de puntos x,y
aleatorios dentro del cuadrado que va de 0 a 1 en los ejes X e Y. Para cada par de puntos
generado, el hilo debe comprobar si ese punto está dentro del círculo de radio 1 centrado en
el origen, verificando si se cumple la condición
x2 + y2 ≤ 1
• Cada hilo llevará la cuenta de cuántos puntos caen dentro del círculo y de cuántos ha
generado en total. Al terminar, calculará la proporción entre ambos valores (puntos dentro
/ puntos totales) y multiplicará ese resultado por 4. Este valor aproximará el número π.
• El número de hilos debe poder variarse utilizando una constante. Los hilos pares serán
los generosos y los impares los competitivos. Los hilos del grupo generoso deben llamar
periódicamente a sched_yield durante sus iteraciones para ceder voluntariamente la CPU,
mientras que los hilos del grupo competitivo no deben hacerlo. Utiliza una constante para
establecer cada cuántas iteraciones llaman a sched_yield, de manera que se pueda modificar
fácilmente para ver el efecto.
• Comprueba que los hilos que hacen llamadas a sched_yield terminan más tarde que los que
no, es decir, que ser amables reduce su rendimiento.
• Mide y muestra el tiempo total de ejecución de cada hilo para poder comparar ambos
comportamientos. Debes imprimir una tabla final donde se muestre, ordenado de menor a
mayor tiempo de ejecución, el número del hilo, el tipo del hilo (generoso o competitivo), el
tiempo de ejecución y la aproximación de π obtenida.*/


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>    
#include <string.h>
#include <sys/time.h>

int num_puntos = 1000000;
int num_hilos=8;//valor por defecto
int freq_yield=10;//valor por defecto

typedef struct {
    int id;
    char tipo[12];   
    double pi;
    double tiempo;   
} resultado;

resultado resultados[10];


void* calculo_competitivo(void* arg) {
    int id = *(int*)arg;
    int aprox=0;
    struct timeval inicio, fin;
    double tiempo;
    gettimeofday(&inicio, NULL);
    for(int i=1; i<=num_puntos; i++) {
        double x = (double)rand() / RAND_MAX;
        double y = (double)rand() / RAND_MAX;
        if (x*x + y*y <= 1.0) {
            aprox++;
        }
    }
    double pi = 4.0 * aprox / num_puntos;
    gettimeofday(&fin, NULL);
    tiempo = (fin.tv_sec - inicio.tv_sec) + (fin.tv_usec - inicio.tv_usec) / 1000000.0;

    resultados[id].id = id;
    resultados[id].pi = pi;
    resultados[id].tiempo = tiempo;
    strcpy(resultados[id].tipo, "competitivo");
        return NULL;
}

void* calculo_generoso(void* arg) {
    int id = *(int*)arg;
    int aprox=0;
    struct timeval inicio, fin;
    double tiempo;
    gettimeofday(&inicio, NULL);
    for(int i=1; i<=num_puntos; i++) {
        if (i % freq_yield == 0) {
            sched_yield();
        }
        double x = (double)rand() / RAND_MAX;
        double y = (double)rand() / RAND_MAX;
        if (x*x + y*y <= 1.0) {
            aprox++;
        }
    }
    double pi = 4.0 * aprox / num_puntos;
    gettimeofday(&fin, NULL);
    tiempo = (fin.tv_sec - inicio.tv_sec) + (fin.tv_usec - inicio.tv_usec) / 1000000.0;

    resultados[id].id = id;
    resultados[id].pi = pi;
    resultados[id].tiempo = tiempo;
    strcpy(resultados[id].tipo, "generoso");
    return NULL;
}


int main(int argc,char* argv[]) {
    if(argc<3){
        printf("Faltan argumentos: num_hilos freq_yield\n");
        return 1;
    }
    num_hilos = atoi(argv[1]);
    freq_yield = atoi(argv[2]);
    pthread_t hilos[num_hilos];
    int ids[num_hilos];
    for(int i=0; i<num_hilos; i++){
        ids[i] = i;
        if (i % 2 == 0) {
            if (pthread_create(&hilos[i], NULL, calculo_generoso, &ids[i]) != 0) {
                perror("Error al crear hilo");
                return 1;
            }
        } else {
            if (pthread_create(&hilos[i], NULL, calculo_competitivo, &ids[i]) != 0) {
                perror("Error al crear hilo");
                return 1;
            }
        }
    }

    for (int i = 0; i < num_hilos; i++) {

        pthread_join(hilos[i], NULL);
    }
        
    for (int i = 0; i < num_hilos - 1; i++) {
        for (int j = i + 1; j < num_hilos; j++) {
            if (resultados[i].tiempo > resultados[j].tiempo) {
                resultado tmp = resultados[i];
                resultados[i] = resultados[j];
                resultados[j] = tmp;
            }
        }
    }


    printf("\n%-10s %-12s %-12s %-10s\n", "Hilo", "Tipo", "Tiempo", "π");
    printf("----------------------------------------------------\n");
    for (int i = 0; i < num_hilos; i++) {
        printf("%-10d %-12s %-12.4f %-10.6f\n",
               resultados[i].id,
               resultados[i].tipo,
               resultados[i].tiempo,
               resultados[i].pi);
    }


    return 0;
}