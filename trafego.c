#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#define ESQUERDA 0
#define DIREITA 1

int capacidade = 0; // Capacidade da ponte
int ocupacao = 0; // Ocupacao atual da ponte
int sentido_atual = -1; // Indica o sentido da via (0 ou 1)
int esperando[2] = {0, 0}; // Numero de veiculos esperando em cada sentido

// Mutex e variavel de condicao
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condicao = PTHREAD_COND_INITIALIZER;

void *carro(void *threadid);

int main(){
    int veiculos;   // Numero de threads
    int retorno = 0;    // Recebe o codigo de retorno
    // Obtem os dados 
    printf("Indique a capacidade da ponte: \n");
    scanf(" %d", &capacidade);
    printf("Indique a quantidade de veiculos: \n");
    scanf(" %d", &veiculos);
    
    srand(time(0)); // Inicia a funcao aleatoria

    pthread_t threads[veiculos]; // Armazena os identificadores das threads
    int *taskids[veiculos]; // Armazena os IDs das threads
    int t, u; // Variaveis de loop

    // Cria as threads
    for(t=0; t<veiculos; t++){
        printf("Na Main: criando veiculo %d\n", t);
        taskids[t] = (int *)malloc(sizeof(int));
        *taskids[t] = t;
        retorno = pthread_create(&threads[t], NULL, carro, (void *) taskids[t]);
        if (retorno){
            printf("ERRO; código de retorno é %d\n", retorno);         
            exit(-1);      
        } 
    }
    for(u = 0; u<veiculos; u++){
        pthread_join(threads[u], NULL);
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&condicao);

    return 0;
}

void *carro(void *threadid){
    int tid = *((int *) threadid);
    int direcao = rand() % 2; // Define a direcao aleatoreamente

    pthread_mutex_lock(&mutex);

    // Comeca a esperar se nao pode entrar na ponte
    esperando[direcao]++; // Incrementa a fila de espera do sentido
    while(ocupacao >= capacidade || (ocupacao >0 && sentido_atual != direcao)){
        pthread_cond_wait(&condicao, &mutex); // Coloca a thread para esperar
    }
    esperando[direcao]--;

    // Entra na ponte
    ocupacao++;
    sentido_atual = direcao;
    printf("Carro %d entrou na ponte no sentido %s. Ocupacao: %d\n",
            tid, direcao == ESQUERDA? "para Esquerda" : "para Direita", ocupacao);

    pthread_mutex_unlock(&mutex);

    // Simula o tempo de travessia
    int passando;
    for(passando = 0; passando < 100; passando++);

    pthread_mutex_lock(&mutex);

    //Sair da ponte
    ocupacao--;
    printf("Carro %d saiu da ponte. Ocupacao: %d\n", tid, ocupacao);

    // Verifica se eh preciso alterar o sentido
    if(ocupacao == 0){
        if(esperando[1 - sentido_atual] > 0){
            sentido_atual = 1 - sentido_atual; // alterna o sentido
        }
        else if( esperando[sentido_atual] == 0){
            sentido_atual = -1; // Ponte vazia, sem sentido especifico
        }
    }

    pthread_cond_broadcast(&condicao);
    pthread_mutex_unlock(&mutex);

    free(threadid);
    pthread_exit(NULL);
}