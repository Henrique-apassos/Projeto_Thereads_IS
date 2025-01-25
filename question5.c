#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define sizeBuffer 80 //Tamanho do buffer

//Prototipo das funcoes
void *executeFunction(void *argument); //Executa a system call
int agendarExecucao(void (*funexec)(void*), void *argument); //Agenda a execucao
void *functionDespachante(void *argument); //Funcao para a thread despachante
int pegarResultadoExecucao(int id); //Pega o resultado
void systemCall(void *threadId); //Sytem call em si

int N = 8; //Numero de processadores
int CountId = 0; //Contador para auxiliar no inserimento de itens no buffer

//Estrutura para a requisicao
typedef struct{
    int id; //Id da requisicao
    void (*funexec)(void*); //System call(function)
    void *argument; //Argumento da system call(function)
}Request;

//Estrutura para o resultado da requisicao
typedef struct{
    int id; //Id do resultado
    int result; //Resultado
    int completionStatus; //Status da requisicao, caso ja tenha sido concluida, marca com 1
}RequestResult;

Request requestBuffer[sizeBuffer]; //Buffer para as requisicoes
RequestResult resultBuffer[sizeBuffer];
int request_initial = 0, request_final = 0; //Índices de início e fim do buffer circular de requisições

//Mutexes para proteger acesso aos buffers de requisições e resultados
pthread_mutex_t mutexRequest = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexResult = PTHREAD_MUTEX_INITIALIZER;
//Variáveis de condição usadas para bloquear threads até que uma condição específica seja atendida (ex.: novos pedidos ou resultados disponíveis
pthread_cond_t condRequest = PTHREAD_COND_INITIALIZER;
pthread_cond_t condResult = PTHREAD_COND_INITIALIZER;

int main(void){

    pthread_t threadDespachante;
    pthread_create(&threadDespachante, NULL, functionDespachante, NULL);

    for(int i = 0;i < sizeBuffer;i++){
        resultBuffer[i].completionStatus = 0;
    }
    srand(time(0)); //gerar uma semente "aleatoria"

    int arrayId[N];
    //Agenda a execucao de cada system call
    for(int i = 0;i < N;i++){
        int *tempId = (int*)malloc(sizeof(int));
        *(tempId) = i;
        arrayId[i] = agendarExecucao(systemCall, tempId);
    }
    //Pegar os resultados
    for(int i = 0;i < N;i++){
        printf("Result of running number %d --> %d\n", i+1, pegarResultadoExecucao(arrayId[i]));
    }
    return 0;
}

void *executeFunction(void *argument){

    Request *request = (Request*)argument; //Faz o cast do argumento transformando em uma requisicao
    request->funexec(request->argument);  //Executa a funcao da requisicao

    pthread_mutex_lock(&mutexResult); //Trava o mutex do buffer de resultado
    resultBuffer[request->id].result = rand() % 150; //Gera um resultado pseudo-aleatorio
    resultBuffer[request->id].completionStatus = 1; //Marca como completo
    pthread_cond_broadcast(&condResult); //Acorda quem estava esperando pela condicao de algum resultado disponivel
    pthread_mutex_unlock(&mutexResult); //Destrava o mutex

    free(request);
}

int agendarExecucao(void (*funexec)(void*), void *argument){

    pthread_mutex_lock(&mutexRequest); //Trava o mutex de requisicao
    int id = CountId++; //Aumenta o CountId;

    Request *request = (Request*)malloc(sizeof(Request)); //Aloca uma nova requisicao

    //Inicaliza a requisao, com seu id, sua funcao de execucao e seru argumento
    request->id = id;
    request->funexec = funexec;
    request->argument = argument;

    requestBuffer[request_final] = *(request); //Insere a requisicao no buffer
    request_final = (request_final + 1) % sizeBuffer; //Aumenta o indice do final(buffer circular)

    pthread_cond_broadcast(&condRequest); //Acorda quem estava esperando pela condicao de algum resultado disponivel
    pthread_mutex_unlock(&mutexRequest); //Destrava o mutex

    return id;
}

void *functionDespachante(void *argument){
    while(1){
        pthread_mutex_lock(&mutexRequest); //Trava o mutex

        //Loop infinito que verifica se há requisições no buffer
        while(request_final == request_initial){
            pthread_cond_wait(&condRequest, &mutexRequest);
        }

        //Remove uma requisição do buffer circular e a processa.
        Request *request = (Request*)malloc(sizeof(Request));
        *(request) = requestBuffer[request_initial];
        request_initial = (request_initial+1) % sizeBuffer; //Atualiza o posicao inicial

        pthread_mutex_unlock(&mutexRequest); //Destrava o mutex

        pthread_t thread;
        pthread_create(&thread, NULL, executeFunction, (void*)request); //Cria uma nova que vai executar a funcao passando a requisicao como argumento
    }
}
int pegarResultadoExecucao(int id){
    pthread_mutex_lock(&mutexResult); //Trava o mutex

    //Se o status da requisicao ainda nao for completo
    while(!resultBuffer[id].completionStatus){
        pthread_cond_wait(&condResult, &mutexResult); //Espera a variavel de condicao de resultado acordar
    }

    int result = resultBuffer[id].result; //Guarda o resultado numa variavel
    pthread_mutex_unlock(&mutexResult); //Destrava o mutex

    return result; //Retorna o resultado
}

void systemCall(void *threadId){
    int *id = (int*)threadId; //Guardando o id numa variavel
    printf("Thread %d executing system call...\n", *(id)); 
    sleep(1);   //Simula um atraso de 1 segundo
    *(id) = *(id) + 1;
}

