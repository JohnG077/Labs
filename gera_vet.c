/*
Programa auxiliar para gerar um vetor de floats 
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX 1001 //valor maximo dos elementos do vetor

double soma_kahan(const float *valores, int n){	// faz uma soma compensada para reduzir o erro de arredondamento
	double soma = 0.0;
	double compensation = 0.0;	//variável de compensação
	
	for (int i = 0; i < n; i++) {
		double y = valores[i] - compensation;	// correção do valor
		double t = soma + y;
		compensation = (t - soma) - y;		// Recuperação da compensação
		soma = t;			// Atualização da soma 
	}
	
	return soma;
}

int main(int argc, char *argv[]) {
    //valida e recebe os valores de entrada
    if(argc != 2) {
       printf("Use: %s <numero de elementos (maior que 0)> \n", argv[0]);
       return 1;
    }
    
    long int n = atol(argv[1]); //qtde de elementos do vetor que serao gerados
    
    if(n<1) {
       printf("ERRO: o numero de elementos deve ser maior que 0 \n");
       return 2;
    }; 
    
    float vetor[n];

    //semente para gerar numeros randomicos
    srand((unsigned)time(NULL));

    //imprime a qtde de elementos do vetor (inteiro)
    printf("Qtde de elementos: %ld\n", n);
 
	//gera e imprime os elementos restantes separado por espaco em branco
    printf("Valores: ");
    for(int i=0; i<n; i++) {
        vetor[i] = (rand() % MAX)/2.5 - 200.0;	// valores de -200 a 200
        printf("%f ", vetor[i]);
        
    }
    
    return soma_kahan(vetor, n);
}
