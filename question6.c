#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#define M 5 /*numero de threads para escrita*/
#define N 10 /*numero de threads para leitura*/

pthread_t leitura[N]; //array de threads usado na leitura do SGBD
pthread_t escrita[M]; //array de threads usado na escrits DO SGBD

pthread_mutex_t mutexbasededados = PTHREAD_MUTEX_INITIALIZER; //mutex usado para bloquear as leituras feitas pelas threads de leitura, enquanto as threads da escrita estao sendo atuados
pthread_cond_t estaEscrevendo = PTHREAD_COND_INITIALIZER;// variavel de condicao, usada para acordar as threads de leitura, apos as threads de escrita termimarem
char basededados[100]; // regiao critica que pode ser acessada pelas threads de leitura e modificadas pelas threads de escrita


void *escricao(void *arg);//funcao de escrita
void *ler(void *arg);//funcao de leitura

int main(void){

for(int i = 0; i < M; i++){ //loop para criacao das threads de escrita 
    pthread_create(&escrita[i], NULL, escricao, (void*)&i);
}

for(int i = 0; i<N; i++){ //loop para criacao das threads de leitura
    pthread_create(&leitura[i], NULL, ler, (void*)&i);
}
 // Espera todas as threads de escrita  terminarem
    for (int i = 0; i < M; i++) {
        pthread_join(escrita[i], NULL);
    }
// espera todas as threads de leitura terminarem
    for (int i = 0; i < N; i++) {
        pthread_join(leitura[i], NULL);
    }

    return 0;
}

void *escricao(void *arg){ //funcao de escrita

    int id = *(int*)arg; //recebimento do argumento (int)
    while(1){ //loop sempre em execucao
    
        pthread_mutex_lock(&mutexbasededados); //bloqueia o acesso das outras threads de escrita, para que apenas uma thread de escrita possa acessar a base de dados
        printf("Escritor %d iniciou a escrita...\n", id);
        sprintf(basededados, "Escritor %d modificou a base de dados...\n", id);
        printf("Escritor %d terminou a escrita...\n", id);

        pthread_cond_broadcast(&estaEscrevendo); //avisa as threads de leitura que a escrita terminou
        pthread_mutex_unlock(&mutexbasededados); //desbloqueia a escrita
        
        sleep(1); //simula a demora na escrita
    }
}

void *ler(void *arg){ //funcao de leitura
    
    int id = *(int*)arg; //recebimento do argumento (int)
    while (1) {
    // Espera ate que a variavel de condicao seja sinalizada
    if (pthread_cond_wait(&estaEscrevendo, &mutexbasededados) != 0) {
        printf("Erro ao esperar a condicao.\n");
        break;
    }

    // Depois que a condicao eh sinalizada, o thread pode acessar a base de dados
    printf("Leitor %d iniciou a leitura...\n", id);
    printf("%s", basededados);
    printf("Leitor %d terminou a leitura...\n", id);

    sleep(1); // Simula a demora na leitura

}


}