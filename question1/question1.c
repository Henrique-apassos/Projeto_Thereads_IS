#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// STRUCT PARA PASSAR OS ARGUMENTOS PARA A THREAD
typedef struct
{
    char *filename; // nome do arquivo
    char *word;     // palavra a ser buscada no arquivo
} ThreadArgs;

// DECLARAÇÃO DE FUNÇÕES
void *searchInFile(void *args);

int main(int argc, char *argv[])
{
    // VERIFICAR SE O USUÁRIO PASSOU OS ARGUMENTOS CORRETAMENTE
    if (argc < 3)
    {
        fprintf(stderr, "Use: %s <palavra> <arquivo1> <arquivo2> ... <arquivoN>\n", argv[0]);
        return 1;
    }

    char *word = argv[1];       // palavra que vai ser buscada
    int num = argc - 2;         // qntd de arquivos como argumento
    pthread_t threads[num];     // vetor de threads
    ThreadArgs threadArgs[num]; // vetor de argumentos para cada thread

    // THREAD PARA CADA ARQUIVO
    for (int i = 0; i < num; i++)
    {
        threadArgs[i].filename = argv[i + 2];
        threadArgs[i].word = word;
        // CRIAÇÃO DA THREAD E VERIFICAÇÃO DE ERRO
        if (pthread_create(&threads[i], NULL, searchInFile, &threadArgs[i]) != 0)
        {
            perror("Erro ao criar a thread");
            return 1;
        }
    }

    // ESPERAR QUE TODAS AS THREADS TERMINEM
    for (int i = 0; i < num; i++)
    {
        pthread_join(threads[i], NULL);
    }
    return 0;
}

// FUNÇÃO DE PESQUISA EXECUTADA POR CADA THREAD
void *searchInFile(void *args)
{
    ThreadArgs *threadArgs = (ThreadArgs *)args; // converte arg recebido

    // passa os parametros para a thread
    char *filename = threadArgs->filename;
    char *word = threadArgs->word;

    // abre o arquivo em modo leitura
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        perror("Erro ao abrir o arquivo"); // erro se nao conseguir
        pthread_exit(NULL);
    }

    char *line = NULL; // armazena uma linha do arquivo
    size_t len = 0;    // inicializa tamanho do buffer para o getline
    ssize_t read;      // numero de linhas lidas pelo getline
    int lineNum = 0;   // contador de linhas

    // LER O ARQUIVO LINHA POR LINHA
    while ((read = getline(&line, &len, file)) != -1)
    {
        lineNum++;
        if (strstr(line, word))
        {
            printf("%s:%d\n", filename, lineNum); // imrpime o nome do arquivo e a linha onde a palavra foi encontrada
        }
    }
    // libera memoria, fecha o arquivo e finaliza a execução da thread
    free(line);
    fclose(file);
    pthread_exit(NULL);
}
