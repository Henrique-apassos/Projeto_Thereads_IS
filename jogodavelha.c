#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


void printjogo(int matriz[3][3]);/*funcao para printar o jogo no terminal*/

/*criar 3 funcoes para 3 threads, cada thread vai checar as linhas horizontais, verticais e diagonais*/
/*caso determinada thread encontrar uma linha horizontai, vertical ou diagonal, é anunciado o jogador que ganhou */
void *horizontal(void *arg);/*checar as linhas horizontais*/
void *vertical(void *arg);/*checar as linhas verticais*/
void *diagonal(void *arg);/*checar as linhas diagonais*/
void chamada(int jogadas1, int jogadas2);  //funcao para criar as threads a cada interacao do loop
int tabela[3][3] = {0}; /*tabela do jogo da velha*/

int jogador1x, jogador1y, jogador2x, jogador2y; /*posicoes dos jogadores*/
pthread_t thread1, thread2, thread3; /*threads para as funcoes horizontais, verticais e diagonais*/
pthread_mutex_t mutex; /*mutex usado para regiao critica (int ganha)*/

/*sao feitas 9 jogadas no total*/
int contador = 0; /*contador para as jogadas*/
int jogadas1 = 0;/*ontador para as jogadas dos jogadores*/
int jogadas2 = 0; /*contador para as jogadas dos jogadores*/
int ganha = 0; /*variavel para verificar se houve um ganhador*/

int main(){
    pthread_mutex_init(&mutex, NULL); /*inicializar mutex*/ 
/*criar um loop para que o jogo seja executado ate que todas as jogadas sejam feitas*/
while(contador < 9){

int repeticao = 0;//variavel usada para evitar repeticao de jogadas

if(jogadas2 >= 6 && ganha == 0){ //mecanismo usada para formular o empate
    printf("Empate \n");
    break;
}
 /*primeiro jogador*/
printf("informe a linha e a coluna do jogador 1\n");

/*mecanismo para evitar preenchimento de local ocupado*/
do{
    if(repeticao == 1){
        printf("posicao ocupada\n");
    }
scanf("%d %d",&jogador1x,&jogador1y);
    repeticao = 1;
}while(tabela[jogador1x][jogador1y] == 1 || tabela[jogador1x][jogador1y] == 2); /*caso a posicao esteja ocupada (==1 ou ==2) o jogador deve escolher outra posicao*/
    repeticao = 0;


if(tabela[jogador1x][jogador1y] == 0){ /*caso a posicao esteja vazia (==0) é preenchido na tabela o numero 1 (= x)*/
   tabela[jogador1x][jogador1y] = 1;

   printjogo(tabela); /*printar o jogo no terminal*/
   jogadas1 ++;
    contador ++;
    if(jogadas1 >= 5 && ganha == 0){//mecanismo usada para formular o empate
    printf("Empate\n");
    break;
     }
   
      chamada(jogadas1, jogadas2); //criar as threads para checar as linhas horizontais, verticais e diagonais
}

 if (ganha) break; /*se houver um ganhador, o loop é quebrado*/

/*segundo jogador*/
printf("informe a linha e a coluna do jogador 2\n");

/*mecanismo para evitar preenchimento de local ocupado*/
do{
    if(repeticao == 1){
        printf("posicao ocupada\n");
    }
scanf("%d %d",&jogador2x,&jogador2y);
    repeticao = 1;
}while(tabela[jogador2x][jogador2y] == 1 || tabela[jogador2x][jogador2y] == 2); /*caso a posicao esteja ocupada (==1 ou ==2) o jogador deve escolher outra posicao*/
    repeticao = 0;

if(tabela[jogador2x][jogador2y] == 0){ /*caso a posicao esteja vazia (==0) é preenchido na tabela o numero 2 (= o)*/
    tabela[jogador2x][jogador2y] = 2;
   printjogo(tabela);//exibir o jogo no terminal
    jogadas2 ++;
     contador ++; 
     
     if(jogadas1 >= 5 && ganha == 0){//mecanismo usada para formular o empate
    printf("Empate \n");
    break;
     }
     chamada(jogadas1, jogadas2);// criar as threads para checar as linhas horizontais, verticais e diagonais
}
 if (ganha) break; /*se houver um ganhador, o loop é quebrado*/ 
}

 pthread_mutex_destroy(&mutex); /* Destruir o mutex */
    return 0;
}


void printjogo(int matriz[3][3]){ /*funcao para printar o jogo no terminal*/
    for(int i=0;i<3;i++){
        for(int j=0;j<3;j++){
            if(matriz[i][j] == 0){
                printf("- ");
            }
            if(matriz[i][j] == 1){
                printf("X ");
            }
            if(matriz[i][j] == 2){
                printf("O ");
            }
        }
        printf("\n");
    }
}

void *horizontal(void *arg){/*funcao que checa se ha um ganhador na horizontal*/

 int (*tabela)[3] = (int (*)[3])arg;/*recebimento do jogo da velha*/

 for(int i=0; i<3; i++){
  
       if(tabela[i][0] == 1 && tabela[i][1] == 1 && tabela[i][2] == 1){ /*checa as linhas horizontais do jogador 1*/
           pthread_mutex_lock(&mutex); /*utilizacao do mutex para o acesso da regiao critica */
            printf("Jogador 1 ganhou na horizontal\n");
              ganha = 1;
            pthread_mutex_unlock(&mutex);
            pthread_exit(NULL);/*finalizando a thread*/
        }
      if(tabela[i][0] == 2 && tabela[i][1] == 2 && tabela[i][2] == 2){ /*checa as linhas horizontais do jogador 2*/
           pthread_mutex_lock(&mutex); /*utilizacao do mutex para o acesso da regiao critica */
            printf("Jogador 2 ganhou na horizontal\n");
              ganha = 1;
             pthread_mutex_unlock(&mutex);
            pthread_exit(NULL);/*finalizando a thread*/
        }

}
}

void *vertical(void *arg){/*funcao que checa se ha um ganhador na vertical*/
   
     int (*tabela)[3] = (int (*)[3])arg;/*recebimento do jogo da velha*/
    
     for(int i=0; i<3; i++){
        
         if(tabela[0][i] == 1 && tabela[1][i] == 1 && tabela[2][i] == 1){/*checa as linhas verticais do jogador 1*/
             pthread_mutex_lock(&mutex); /*utilizacao do mutex para o acesso da regiao critica */
                printf("Jogador 1 ganhou na vertical\n");
                  ganha = 1;
                    pthread_mutex_unlock(&mutex);
                pthread_exit(NULL);/*finalizando a thread*/
            }
            if(tabela[0][i] == 2 && tabela[1][i] == 2 && tabela[2][i] == 2){/*checa as linhas verticais do jogador 2*/
                 pthread_mutex_lock(&mutex); /*utilizacao do mutex para o acesso da regiao critica */
                printf("Jogador 2 ganhou na vertical\n");
                  ganha = 1;
                pthread_mutex_unlock(&mutex);
                pthread_exit(NULL);/*finalizando a thread*/
            }
        
    }
    }

void *diagonal(void *arg){/*funcao que checa se ha um ganhador na diagonal*/

   int (*tabela)[3] = (int (*)[3])arg;/*recebimento do jogo da velha*/

            if((tabela[0][0] == 1 && tabela[1][1] == 1 && tabela[2][2] == 1) || 
            (tabela[0][2] == 1 && tabela[1][1] == 1 && tabela[2][0] == 1)){ /*checa as linhas diagonais do jogador 1*/
                 pthread_mutex_lock(&mutex); /*utilizacao do mutex para o acesso da regiao critica */
                printf("Jogador 1 ganhou na diagonal\n");
                  ganha = 1;
                    pthread_mutex_unlock(&mutex);
                pthread_exit(NULL); /*finalizando a thread*/
            }
            if((tabela[0][0] == 2 && tabela[1][1] == 2 && tabela[2][2] == 2) || 
            (tabela[0][2] == 2 && tabela[1][1] == 2 && tabela[2][0] == 2)){ /*checa as linhas diagonais do jogador 1*/
                 pthread_mutex_lock(&mutex); /*utilizacao do mutex para o acesso da regiao critica */
                printf("Jogador 2 ganhou na diagonal\n");
                  ganha = 1;
                    pthread_mutex_unlock(&mutex);
                pthread_exit(NULL); /*finalizando a thread*/
            }
    }

    void chamada(int jogadas1, int jogadas2){// funcao para criar as threads
        if(jogadas1 >= 2 || jogadas2 >= 2 ) {
            pthread_t thread1, thread2, thread3;
            pthread_create(&thread1, NULL, horizontal, (void *)tabela); //criacao da thread 1 designada para a funcao horizontal
            pthread_create(&thread2, NULL, vertical, (void *)tabela); //criacao da thread 2 designada para a funcao vertical
            pthread_create(&thread3, NULL, diagonal, (void *)tabela); //criacao da thread 3 designada para a funcao diagonal 
            pthread_join(thread1, NULL); //aguardar a conclusao da thread 1 
            pthread_join(thread2, NULL); //aguardar a conclusao da thread 2
            pthread_join(thread3, NULL); //aguardar a conclusao da thread 3
        }
    }
