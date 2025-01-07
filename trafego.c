#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void *carro(void *threadid);

int main(){
    int capacidade; // Capacidade da ponte
    int veiculos;   // Numero de threads
    int retorno;    // Recebe o codigo de retorno
    // Obtem os dados 
    printf("Indique a capacidade da ponte: \n");
    scanf(" %d", &capacidade);
    printf("Indique a quantidade de veiculos: \n");
    scanf(" %d", &veiculos);

    pthread_t threads[veiculos]; // Armazena os identificadores das threads
    int *taskids[veiculos]; // Armazena os IDs das threads
    int t, u; // Variaveis de loop

    // Cria as threads
    for(t=0; t<veiculos; t++){
        printf("Na Main: criando veiculo %d\n", t);
        taskids[t] = (int *)malloc(sizeof(int));
        *taskids[t] = t;
        pthread_create(&threads[t], NULL, carro, (void *) taskids[t]);
    }

    return 0;
}

void *carro(void *threadid){
    pthread_exit(NULL);
}