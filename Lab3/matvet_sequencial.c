/* Multiplicacao de matrizes */
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include "timer.h"

float *mat1; //matriz 1 de entrada
float *mat2; //matriz 2 de entrada
/* multiplicação: mat1 x mat2 */

float *saida; //matriz de saida
int L, C, N; // dimensões das matrizes 1 e 2 (N é o valor em comum entre coluna1 e linha2, L é a linha1 e C é a coluna2)
/*
	* Obs.: matriz 1 = LxN
	* 		matriz 2 = NxC
	*		Saida = LxC
*/   




void chama_arquivo(char * arq, int linha, int coluna, float * mat)
{
   FILE * descritorArquivo; //descritor do arquivo de entrada
   size_t ret; //retorno da funcao de leitura no arquivo de entrada
   
	//abre o arquivo para leitura binaria
   descritorArquivo = fopen(arq, "rb");
   if(!descritorArquivo) {
      fprintf(stderr, "Erro de abertura do arquivo\n");
      exit(2);
   }

   //le as dimensoes da matriz
   ret = fread(&linha, sizeof(int), 1, descritorArquivo);
   if(!ret) {
      fprintf(stderr, "Erro de leitura das linhas da matriz \n");
      exit(3);
   }
   
   ret = fread(&coluna, sizeof(int), 1, descritorArquivo);
   if(!ret) {
      fprintf(stderr, "Erro de leitura das colunas da matriz\n");
      exit(3);
   }
   
   int dim = linha * coluna; //calcula a qtde de elementos da matriz 

   //alocacao de memoria para a matriz
   mat = (float *) malloc(sizeof(float) * dim);
   if (mat == NULL) {
   	fprintf(stderr, "Erro de alocacao da memoria da matriz\n"); 
   	exit(4);
   }
   
   //carrega a matriz de elementos do tipo float do arquivo
   ret = fread(mat, sizeof(float), dim, descritorArquivo);
   if(ret < dim) {
      fprintf(stderr, "Erro de leitura dos elementos da matriz\n");
      exit(4);
   }
}




//fluxo principal
int main(int argc, char* argv[]) {
   float *saida; //matriz que salvará a multiplicação das matrizes
   int lin1, col1; //dimensoes da matriz 1
   int lin2, col2; //dimensoes da matriz 2

   double inicio, fim, delta;
   
   
   GET_TIME(inicio);
   //leitura e avaliacao dos parametros de entrada
   if(argc<3) {
      printf("Digite: %s <arquivo binario da matriz 1> <arquivo binario da matriz 2>\n", argv[0]);
      return 1;
   }


   // Lê a matriz 1
   chama_arquivo(argv[1], lin1, col1, mat1);
   
   // Lê a matriz 2
   chama_arquivo(argv[2], lin2, col2, mat2);
   
   if (col1 != lin2)
   {
   	  fprintf(stderr, "Erro! multiplicacao invalida! linha da matriz 1 eh diferente coluna da matriz 2 (%d != %d)\n", col1, lin2); 
   	  return 5;
   }
	
	L = lin1;
	C = col2;
	N = col1;

   saida = (float *) malloc(sizeof(float) * L * C);  
   if (saida == NULL) 
   {
   	  fprintf(stderr, "Erro de alocacao da memoria da matriz de saida\n"); 
   	  return 4;
   }

   //inicializacao das estruturas de dados da matriz de saida
   for(int i = 0; i < L; i++) {
      for(int j = 0; j < C; j++)
         saida[i] = 0.0;
   }
   
   GET_TIME(fim);
   delta = fim - inicio;
   printf("Tempo inicializacao:%lf\n", delta);


//////////////////////////////////////////////////////////////////////////

   
   //multiplicacao das matrizes (matriz 1 (LxN) * matriz 2 (NxC) == matriz 3 (LxC)
   GET_TIME(inicio);
	
	for (int i = 0; i < L; i++){
		for (int j = 0; j < C; j++){
			for (int k = 0; k < N; k++)
				saida[i*L+j] += mat1[i*N+k] * mat2[k*C+j];
				// É igual a saida[i][j] += mat1[i][k] * mat2[k][j]
		}
	}

   GET_TIME(fim)   
   delta = fim - inicio;
   printf("Tempo multiplicacao: %lf\n", delta);

   //liberacao da memoria
   GET_TIME(inicio);
   free(mat1);
   free(mat2);
   free(saida);
   GET_TIME(fim)   
   delta = fim - inicio;
   printf("Tempo finalizacao:%lf\n", delta);
   return 0;
}
