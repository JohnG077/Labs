#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

int * vetor;	// inicializa o vetor como uma variável global


typedef struct {
	int idThread, inicio, fim;	// armazena o id, o valor inicial do intervalo e o valor final do intervalo
} ThreadArgs;


// Função que gera os valores e os divide em intervalos
void *eleva(void *args) {
	ThreadArgs *thread_args = (ThreadArgs *)args;
	printf("\nThread %d gerando valores da posicao %d ate %d", thread_args->idThread, thread_args->inicio, thread_args->fim);	
	for (int i = thread_args->inicio; i <= thread_args->fim; i++)
		vetor[i] *= vetor[i];
	
	pthread_exit(NULL);
}


// gera números aleatórios no vetor 
void geraNumero(int tam)
{
	srand(time(NULL));	// define uma semente da função rand() para gerar números aleatórios
	
	vetor = malloc(tam*sizeof(int));
	if (vetor == NULL)
	{
		printf("ERRO DE ALOCACAO DE MEMORIA!\n");
		exit(-1);
	}
	
	
	printf("vetor inicial: ");
	for (int i = 0; i < tam; i++)
	{
		vetor[i] = (rand() % 200) - 99;	// seleciona um número aleatório entre -99 e 100
		printf("%d ", vetor[i]);
	}
}


int main(int argc, char* argv[])
{
	if (argc != 3 || atoi(argv[1]) > atoi(argv[2]))
	{
		printf("ERRO! Digite \"%s <M> <N>\"\nM = numero de threads\tN = numero de valores no vetor\nOBS.: M <= N\n\n", argv[0]);
		exit(-1);
	}
	
	int M = atoi(argv[1]), N = atoi(argv[2]);
	
	pthread_t threads[M]; // gera os ids das threads
	ThreadArgs *vals = malloc (M * sizeof(ThreadArgs));	//recebe os argumentos para a thread
	if(vals == NULL)
	{
		printf("ERRO DE ALOCACAO DE MEMORIA!\n");
		exit(-1);
	}
	
	
	geraNumero(N);
	
	// calculando o tamanho dos intervalos das threads e seus pontos iniciais e finais
	int intervalo = N / M, resto = N % M, start = 0, end;
	
	for (int i = 0; i < M; i++)
	{
		end = start + intervalo - 1 + (i < resto ? 1 : 0);
		/*	Como temos M threads para N valores e todas as threads devem
			ser usadas, M <= N, mas podemos ter valores que não são 
			divisíveis entre si, por exemplo, M = 7 e N = 9. 
			
			Nesses casos, há algumas threads que gerariam valores a mais 
			que outras (Na situação dada acima -> 9/7 = 1, sobrando 2. 
			Ou seja, 2 threads gerariam 2 valores e o resto gerará apenas 
			1 valor), portanto, pegamos o resto da divisão para definir 
			esse valor, que será 1 a mais dividido pelas threads, ou seja, 
			o número de threads que terão um valor a mais para executar 
			são iguais ao número do resto (por isso a comparação com o
			resto, já que o i vai de 0 até resto-1).
			
			Já o "start + intervalo - 1" é subtraido por 1 para impedir 
			que ultrapasse o limite do array	
   		*/
		
		vals[i].idThread = i+1;
		vals[i].inicio = start;
		vals[i].fim = end;
		
		pthread_create(&threads[i], NULL, eleva, (void *)&vals[i]);
		
		start = end + 1;
	}
	
	for (int i = 0; i < M; i++)
		pthread_join(threads[i], NULL);
	
	printf("\nNovo vetor: ");
	for (int i = 0; i < N; i++)
		printf("%d ", *(vetor+i));
	
	
	free(vetor);
	free(vals);
	
	
	return 0;
}
