//Encontra o menor e o maior valor em um vetor de float
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

//escopo global (par ser compartilhado com as threads)
float *vetor; //vetor de entrada

//define o tipo de dado de entrada das threads
typedef struct {
   	int id; //id da thread
   	long int tamBloco; //tamanho do bloco (cada thread processa um bloco)
	float soma;
} tArgs;

//fluxo das threads
void * tarefa(void * arg) {
   tArgs *args = (tArgs *) arg; 
   
   char comando[100];
   snprintf(comando, sizeof(comando), "./%s %ld", PROGRAM_NAME, args->tamBloco);
   FILE *pipe = popen(comando, "r");
   if (pipe == NULL){
   	printf("Erro ao chamar o programa auxiliar.\n");
   	exit(1);
   }
   
   // Pega os valores gerados pelo programa auxiliar e os soma
   while (fscanf(pipe, "%f", vetor) == 1) {args->soma += *vetor;}
   
   // Fecha o pipe
   pclose(pipe);

   //retorna o resultado da soma local
   pthread_exit((void *) vetor); 
}

//fluxo principal
int main(int argc, char *argv[]) {
   if(argc != 2) {
       fprintf(stderr, "Digite: %s <numero threads>\n", argv[0]);
       return 1; 
   }
   
   
   long int dim; //dimensao do vetor de entrada
   long int tamBloco; //tamanho do bloco de cada thread 
   int nthreads = atoi(argv[1]); //numero de threads que serao criadas

   pthread_t *tid; //vetor de identificadores das threads no sistema

   //recebe e valida os parametros de entrada (dimensao do vetor, numero de threads)
   printf("nthreads=%d\n", nthreads); 

   //carrega o vetor de entrada
   printf("Dimensao do vetor: ");
   scanf("%ld", &dim); //primeiro pergunta a dimensao do vetor
   printf("dim=%ld\n", dim); 

   //aloca o vetor de entrada
   vetor = (float*) malloc(sizeof(float)*dim);
   if(vetor == NULL) {
      fprintf(stderr, "ERRO--malloc\n");
      return 2;
   }


   //cria as threads
   tid = (pthread_t *) malloc(sizeof(pthread_t) * nthreads);
   if(tid==NULL) {
      fprintf(stderr, "ERRO--malloc\n");
      return 3;
   }
   tamBloco = dim/nthreads;  //tamanho do bloco de cada thread 
   if(!tamBloco) 
      printf("\nA quantidade de threads eh maior que a quantidade de elementos, a execucao sera sequencial!\n");
   	for(int i=0; i<nthreads; i++) {
       //aloca e preenche argumentos para thread
       tArgs *args = (tArgs*) malloc(sizeof(tArgs));
       if (args == NULL) {
          printf("--ERRO: malloc()\n"); 
          pthread_exit(NULL);
       }
       args->id = i; 
       args->tamBloco = tamBloco; 
       if(pthread_create(tid+i, NULL, tarefa, (void*) args)){
          fprintf(stderr, "ERRO--pthread_create\n");
          return 5;
       }
	}
	
   float sumtot = 0;
   
   //processa a parte final do vetor
   if(dim%nthreads) {	// Se dim não é divisível por nthreads
   		puts("\nAdicionando tarefas no Main\n");
        		// Fazer soma sequencial
   		char comando[100];
   		snprintf(comando, sizeof(comando), "./%s %ld", PROGRAM_NAME, (dim%nthreads));
   		FILE *pipe = popen(comando, "r");
   		if (pipe == NULL){
   			printf("Erro ao chamar o programa auxiliar.\n");
   			exit(1);
   		}
   		
   		while (fscanf(pipe, "%f", vetor) == 1) {sumtot += *vetor;}
   		pclose(pipe);
	} 

   //aguarda o termino das threads
   for(int i=0; i<nthreads; i++) {
      if(pthread_join(*(tid+i), (void**) &sumtot)){
         fprintf(stderr, "ERRO--pthread_create\n");
         return 6;
      }
   }

   // Exibe os resultados
   printf("\nSoma: %f\n", sumtot);

   //libera as areas de memoria alocadas
   free(vetor);
   free(tid);

   return 0;
}
