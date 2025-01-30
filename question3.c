#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

#define DIREITA 0
#define ESQUERDA 1
#define CAPACIDADE 10 // Maximo de veiculos no mesmo sentido

typedef enum{
    DIR = DIREITA,
    ESQ = ESQUERDA
} Direcao;

// Struct para controle de estado da ponte
typedef struct{
    int ocupacao;
    Direcao sentido_atual;
    int esperando[2];
} Ponte;

// Variaveis globais
Ponte ponte = {
    .sentido_atual = -1,
    .ocupacao = 0,
    .esperando = {0, 0}
};
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_direita = PTHREAD_COND_INITIALIZER; 
pthread_cond_t cond_esquerda = PTHREAD_COND_INITIALIZER;


// Funcoes e threads
void *carro(void *threadid);
int pode_entrar(Direcao sentido);

int main(){
    int veiculos;

    printf("Quantidade de veiculos: \n");
    scanf("%d", &veiculos);

    srand(time(0));
    pthread_t threads[veiculos];
    int *taskids[veiculos];
    int rc; // Recebe o codigo de retorno
    int t, u; // variaveis de loop
    // Cria Threads (veiculos)
    for(t=0; t < veiculos; t++){
        printf("Na main: criando veiculo %d\n", t);
        taskids[t] = (int *) malloc(sizeof(int));
        *taskids[t] = t;
        rc = pthread_create(&threads[t], NULL, carro, (void *) taskids[t]);
        if(rc){
            printf("ERRO: codigo de rotorno eh %d\n", rc);
            exit(-1);
        }
    }

    //Espera a conclusao
    for(u=0; u<veiculos; u++){
        pthread_join(threads[u], NULL);
    }

    // Destroi recursos de sincronizacao
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_direita);
    pthread_cond_destroy(&cond_esquerda);

    return 0;
}

// Verifica se um carro pode entrar na ponte
int pode_entrar(Direcao sentido){
    if((ponte.ocupacao < CAPACIDADE) && (ponte.sentido_atual == -1 || ponte.sentido_atual == sentido))
        return 1;
    else
        return 0;
}

// Logica do veiculo
void *carro(void *threadid){
    int tid = *((int *) threadid);
    Direcao dir = rand() %2; // Direcao aleatoria

    pthread_mutex_lock(&mutex); // Entra na regiao critica
    ponte.esperando[dir]++; // Registra na fila de espera

    // Aguarda ate poder entrar
    while(pode_entrar(dir) == 0){
        if(dir == ESQ){
            pthread_cond_wait(&cond_esquerda, &mutex);
        }
        else{
            pthread_cond_wait(&cond_direita, &mutex);
        }
    }

    // Atualiza o estado da ponte
    ponte.esperando[dir]--;
    ponte.ocupacao++;
    if(ponte.sentido_atual == -1){
        ponte.sentido_atual = dir; // Primeiro carro define o sentido
    }

    printf("Carro %d ENTROU (sentido: %s). Ocupacao: %d\n", tid, (dir == ESQ)? "ESQUERDA":"DIREITA", ponte.ocupacao);

    pthread_mutex_unlock(&mutex); // Sai da regiao critica

    // Simula a travessia
    usleep(100000);

    pthread_mutex_lock(&mutex); // Volta para a regiao critica
    ponte.ocupacao--;

    printf("Carro %d SAIU. Ocupacao restante: %d\n", tid, ponte.ocupacao);

    // Logica de alternancia de sentido
    if(ponte.ocupacao == 0){
        if(ponte.esperando[DIR] > ponte.esperando[ESQ]){
            ponte.sentido_atual = DIR;
            pthread_cond_broadcast(&cond_direita); // Acorda todos do novo sentido
        }
        else if(ponte.esperando[ESQ] > ponte.esperando[DIR]){
            ponte.sentido_atual = ESQ;
            pthread_cond_broadcast(&cond_esquerda);
        }
        else{
            ponte.sentido_atual = -1; // Ponte vazia
        }
    }
    else{
        // Notifica apenas um carro do mesmo sentido
        if(ponte.sentido_atual == ESQ){
            pthread_cond_signal(&cond_esquerda);
        }
        else{
            pthread_cond_signal(&cond_direita);
        }
    }

    pthread_mutex_unlock(&mutex); // sai da regiao critica
    free(threadid);
    pthread_exit(NULL); // Encerra a thread

}