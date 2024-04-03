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
} tArgs;


//fluxo das threads
void * tarefa(void * arg) {
   tArgs *args = (tArgs *) arg; 
   
   float *somaLocal = (float *) malloc (sizeof(float));
   
   long int ini = args->id * args->tamBloco; //elemento inicial do bloco da thread
   long int fim = ini + args->tamBloco; //elemento final(nao processado) do bloco da thread   
   

   for (long int i = ini; i < fim; i++)
   {
   	*somaLocal += vetor[i];
   }	
	printf("%f ", *somaLocal);
	
   //retorna o resultado da soma local
   pthread_exit((void *) somaLocal); 
}

//fluxo principal
int main(int argc, char *argv[]) {
   long int dim; //dimensao do vetor de entrada
   long int tamBloco; //tamanho do bloco de cada thread 
   int nthreads; //numero de threads que serao criadas

   pthread_t *tid; //vetor de identificadores das threads no sistema
   float *retorno; //valor de retorno das threads
   float total = 0.0;	   // valor total de todos os elementos do vetor


   //recebe e valida os parametros de entrada (dimensao do vetor, numero de threads)
   if(argc != 2) {
       fprintf(stderr, "Digite: %s <numero threads>\n", argv[0]);
       return 1; 
   }
   nthreads = atoi(argv[1]);
   printf("nthreads=%d\n", nthreads); 

   //carrega o vetor de entrada
   scanf("%ld", &dim); //primeiro pergunta a dimensao do vetor
   printf("dim=%ld\n", dim); 

   //aloca o vetor de entrada
   vetor = (float*) malloc(sizeof(float)*dim);
   if(vetor == NULL) {
      fprintf(stderr, "ERRO--malloc\n");
      return 2;
   }
   //preenche o vetor de entrada
   for(long int i=0; i<dim; i++)
      scanf("%f", &vetor[i]);

   //cria as threads
   tid = (pthread_t *) malloc(sizeof(pthread_t) * nthreads);
   if(tid==NULL) {
      fprintf(stderr, "ERRO--malloc\n");
      return 3;
   }
   tamBloco = dim/nthreads;  //tamanho do bloco de cada thread 
   if(!tamBloco) // se dim/nthreads < 1 (isto é, se nthreads > dim), faz sequencial
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
       if(pthread_create(tid+i, NULL, tarefa, (void*) args)){	// se != 0, dá erro (retornar 0 indica que funcionou)
          fprintf(stderr, "ERRO--pthread_create\n");
          return 5;
       }
   }
   
   //processa a parte final do vetor
   float soma = 0.0;	// Pega a soma da parte que ficou no Main
   if(dim%nthreads) {
      puts("\nMain com tarefa");
      for(long int i=dim-(dim%nthreads); i<dim; i++) {
          soma += vetor[i];
      }
      printf("%f ", soma);
   } 

   //aguarda o termino das threads
   for(int i=0; i<nthreads; i++) {
      if(pthread_join(*(tid+i), (void**) &retorno)){
         fprintf(stderr, "ERRO--pthread_create\n");
         return 6;
      }
      
      total += *retorno;
   }
	
   total += soma;

   printf("\ntotal: %f\n", total);

   //libera as areas de memoria alocadas
   free(vetor);
   free(tid);

   return 0;
}
