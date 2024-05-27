#include <stdio.h>
#include <math.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>


// Variáveis globais

sem_t slotCheio, slotVazio; // sincronização por condição
sem_t mutexGeral;			// sincronização entre produtores e consumidores
int *Buffer;
unsigned long int N;			// quantidade de valores no arquivo
unsigned long int M;						// Tamanho do buffer
char * arqv;				// o arquivo de entrada que será lido
FILE * descritorArquivo; 	//descritor do arquivo de entrada
size_t ret; 				//retorno da funcao de leitura no arquivo de entrada
long long int* valores;		// é o array dos elementos
int *lista_primos;			// lista de quantos primos tem em cada thread
int vencedor = -1;			// thread vencedora
long long int primos;		// quantidade de primos do arquivo de entrada

typedef struct{
	int id;
	int quant_primos;	// quantidade de primos da thread
} tArgs;



// Funções das Threads

int ehPrimo(long long int n) {
	int i;
	if (n<=1) return 0;
	if (n==2) return 1;
	if (n%2==0) return 0;
	for (i=3; i< (long long int) sqrt((double) n)+1; i+=2)
		if (n%i==0) return 0;
	return 1;
}

void Insere(int item) {
	static int in = 0;
	sem_wait(&slotVazio);
	sem_wait(&mutexGeral);
	Buffer[in] = item;
	in = (in+1) % M;
	sem_post(&mutexGeral);
	sem_post(&slotCheio);
}


int Retira(void) {
	int item;
	static int out=0;
		
	sem_wait(&slotCheio);
	sem_wait(&mutexGeral);
	item = Buffer[out];
	Buffer[out] = 0;
	out = (out+1) % M;
	sem_post(&mutexGeral);
	sem_post(&slotVazio);
	return item;
}



// Threads

void *produtor (void *arg){
	
   	ret = fread(&valores, sizeof(unsigned long int), N, descritorArquivo);	// Lê os elementos do arquivo
   	if(!ret) {
      	fprintf(stderr, "Erro de leitura dos valores\n");
      	exit(3);
   	}	
   	
   	
   	for (unsigned long int i = 0; i < N; i++)
	{
		Insere(valores[i]);
	}
	
	pthread_exit(NULL);
}


void *consumidor(void * arg){
	tArgs * args = (tArgs *) arg;

	for (unsigned long int i = 0; i < N; i++)
	{
		printf("Consumidor %lu funcionando?\n", i);
		valores[i] = Retira();						// Pega o elemento a ser checado
		args->quant_primos += ehPrimo(valores[i]);	// checa se é primo
		printf("Consumidor SIM!\n\n");
	}

	pthread_exit(NULL);
}


// Função principal

int main(int argc, char *argv[])
{

	if (argc < 4){
		printf("Erro de execucao!\nAo executar o programa, por favor, digite: \n%s <quant_threads(consumidoras)> <tam_buffer> <arqv_entrada>\n", argv[0]);
		return 1;
	}
	
	// Declarando as variáveis dadas pela entrada
	
	int nthreads = atoi(argv[1]);	// Quantidade de threads (consumidoras)
	M = atoi(argv[2]);				// Tamanho do Buffer
	arqv = argv[3];					// o arquivo de entrada que será lido


	// Alocando espaço para as threads
	pthread_t *threads = malloc ((nthreads+1) * sizeof(pthread_t));
	if (threads == NULL) {
		printf("Erro ao alocar memória para as threads.\n");
		return 1;
	}
	
	
	tArgs *tdata = malloc((nthreads+1) * sizeof(tArgs));
	if (tdata == NULL){
		printf("Erro ao alocar memória para os dados das threads.\n");
		return 1;
	}

   
	//abre o arquivo para leitura binaria
   	descritorArquivo = fopen(arqv, "rb");
   	if(!descritorArquivo) {
      	fprintf(stderr, "Erro de abertura do arquivo\n");
      	exit(1);
   	}
	
   	//le a quantidade de valores inteiros positivos
   	ret = fread(&N, sizeof(unsigned long int), 1, descritorArquivo);
   	if(!ret) {
      	fprintf(stderr, "Erro de leitura na quantidade de valores \n");
      	exit(2);
   	}
   	

	// Inicializando os semáforos e threads
	sem_init(&mutexGeral, 0, 1);
	sem_init(&slotCheio, 0, 0);
	sem_init(&slotVazio, 0, N);
	
	
	
	tdata[0].id = 0;
	pthread_create(&threads[0], NULL, produtor, (void *) arqv);	// Produtor

	sleep(2);
	
	for(int i = 1; i <= nthreads; i++)	// Consumidor
	{
		tdata[i].id = i;
		tdata[i].quant_primos = 0;
		pthread_create(&threads[i], NULL, consumidor, (void *) tdata);
	}
	
	// Esperando todas as threads terminarem para ver quem encontrou mais primos
	for (int i = 0; i <= nthreads; i++)
	{
		pthread_join(threads[i], NULL);
	}
	
	// Obtendo a quantidade de primos
   	ret = fread(&primos, sizeof(unsigned long int), 1, descritorArquivo);
   	if(!ret) {
      	fprintf(stderr, "Erro de leitura da quantidade de primos\n");
      	exit(4);
   	}	
	
	// Checando quem encontrou mais primos
	for (int i = 0; i <= nthreads; i++)
	{
		printf("A thread %d encontrou %d primos", tdata[i].id, tdata[i].quant_primos);
		if (tdata[i].quant_primos > vencedor)
			vencedor = tdata[i].id;
	}
	
	printf("O vencedor foi a thread %d\n", vencedor);
	
	free(threads);
	free(tdata);
	return 0;
}
