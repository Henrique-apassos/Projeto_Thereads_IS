#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

typedef struct Element{
    int value;
    struct Element *prox;
}Element;

typedef struct blockingQueue{
    unsigned int sizeBuffer, statusBuffer;
    Element *head, *last;

    pthread_cond_t notFull; //Variavel para a condição do buffer não estar cheio
    pthread_cond_t notEmpty; //Variavel para a condição do buffer não estar vazio
    pthread_mutex_t myMutex; //Mutex para a região critica(buffer)

}BlockingQueue;

BlockingQueue *newBlockingQueue(unsigned int SizeBuffer); //Funcao que inicializa a fila
void putBlockingQueue(BlockingQueue* Q, int newValue); //Funcao que colocar um elemento na fila
int takeBlockingQueue(BlockingQueue *Q); //tira um elemento da fila
void *producer(void* threadID); //funcao onde o produtor produz elementos
void *consumer(void* threadID); //funcao onde o consumidor retira elementos

BlockingQueue *blockingQueue; //Fila bloqueante criada
unsigned P = 2; //Qtd de produtores
unsigned C = 2; //Qtd de Consumidores
unsigned B = 4; //tamanho do buffer

int main(){

    pthread_t producers[P], consumers[C]; //vetores para as thread consumidoras e produtoras
    blockingQueue = newBlockingQueue(B); //criando a fila bloqueante

    //Criando as thread consumidores e produtoras
    for(int i = 0;i < P;i++){
        int *threadID = malloc(sizeof(int)); 
        *(threadID) = i+1;
        pthread_create(&producers[i], NULL, producer, threadID);
    }
    for(int j = 0;j < P;j++){
        int *threadID = malloc(sizeof(int)); 
        *(threadID) = j+1;
        pthread_create(&consumers[j], NULL, consumer, threadID);
    }
    for(int i = 0;i < P;i++)pthread_join(producers[i], NULL);
    for(int j = 0;j < C;j++)pthread_join(consumers[j], NULL);

    free(blockingQueue);
    pthread_exit(NULL);
}

BlockingQueue *newBlockingQueue(unsigned int SizeBuffer){

    //Alocando uma nova Fila Bloqueante
    BlockingQueue *blockingQueue = (BlockingQueue*)malloc(sizeof(BlockingQueue));
    //Inicializando o header e o last como nulos
    blockingQueue->head = NULL; blockingQueue->last = NULL;

    blockingQueue->statusBuffer = 0; //tamanho atual do buffer 
    blockingQueue->sizeBuffer = SizeBuffer; //tamanho limite passado como argumento

    //Inicializando o mutex e as variaveis de condicao
    pthread_mutex_init(&blockingQueue->myMutex, NULL);
    pthread_cond_init(&blockingQueue->notEmpty, NULL);
    pthread_cond_init(&blockingQueue->notFull, NULL);

    //Retorna a Fila Bloqueante
    return blockingQueue;
}

void putBlockingQueue(BlockingQueue* Q, int newValue){
    
    //Trava o mutex
    pthread_mutex_lock(&Q->myMutex);

    //Se o buffer estiver cheio espera ele nao estar, sinalizado pela variavel de condicao
    if(Q->sizeBuffer == Q->statusBuffer){
        printf("Block queue full, producer waiting. \n");
        pthread_cond_wait(&Q->notFull, &Q->myMutex); //Espera a variavel de condicao e libera o mutex
    }

    //Aloca um novo elemento e inicializa ele com seu valor passado como argumento
    Element *newElement = (Element*)malloc(sizeof(Element));
    newElement->prox = NULL; //proximo nulo, pois ele e o ultimo
    newElement->value = newValue;

    //Se o last da fila for nulo, significa que nao tem nenhum elemento, logo o header sera o novo elemento, caso contrario o proximo do ultimo sera esse novo elemento
    Q->last == NULL ? (Q->head = newElement) : (Q->last->prox = newElement);
    Q->statusBuffer++;//Aumenta o buffer
    Q->last = newElement;//O ultimo e o novo elemento
    pthread_cond_broadcast(&Q->notEmpty); //Acorda todas as threads que estao esperando na condicao notFull
    pthread_mutex_unlock(&Q->myMutex); //Destrava o mutex
}

int takeBlockingQueue(BlockingQueue *Q){

    pthread_mutex_lock(&Q->myMutex); //Trava o mutex

    //Se o buffer estiver vazio
    while(Q->statusBuffer == 0){ 
        printf("Empty queue. Consumers waiting...\n");
        pthread_cond_wait(&Q->notEmpty,&Q->myMutex); //Espera a variavel de condicao sinalizar que nao estar vazio e libera o mutex
    }

    //Armazena o header em uma variavel
    Element *element = Q->head;
    int value = element->value; //Guardamos o valor dela
    Q->head = element->prox; //Atualiza o header na fila bloqueante

    //Se o header for nulo, entao nao tem nenhum elemento
    if(Q->head == NULL){
        Q->last = NULL; //logo o utlimo tambem e nulo
    }
    Q->statusBuffer--; //diminui o statusBuffer, afinal retiramos um elemento

    pthread_cond_broadcast(&Q->notFull); //Acorda todas as threads que estao esperando na condicao notFull
    pthread_mutex_unlock(&Q->myMutex); //Destrava o mutex

    free(element); 
    return value; //Retorna o valor
}

void *producer(void* threadID){

    long* id = (long*)threadID; //variavel para id
    while(1){
        int value = rand() % 200; //gerando um inteiro
        printf("Thread producing ID:%ld producing integer:%d\n", *(id), value);
        putBlockingQueue(blockingQueue, value); //colocando o valor na fila
        sleep(2); //Simulando um atraso de 2 segundos
    }
}

void *consumer(void* threadID){

    long* id = (long*)threadID; //variavel para id
    while(1){
        int value = takeBlockingQueue(blockingQueue); //Pegando o valor retirado
        printf("Thread consuming ID:%ld consuming integer:%d\n", *(id), value);
        sleep(2); //Simulando um atraso de 2 segundos
    }
}

