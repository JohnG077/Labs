#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>


int ehPrimo(long long int n) {
	int i;
	if (n<=1) return 0;
	if (n==2) return 1;
	if (n%2==0) return 0;
	for (i=3; i< (long long int) sqrt((double) n)+1; i+=2)
		if (n%i==0) return 0;
	return 1;
}


int main(int argc, char * argv[])
{
	unsigned long int N, primos = 0;
	if (argc != 2 || atoi(argv[1]) < 1){
		printf("Erro! digite %s <numero inteiro positivo>\n", argv[0]);
		return 1;
	}
	
	N = atoi(argv[1]);
	
	printf("%lu ", N);
	
	srand(time(NULL));
	
	// Gera e printa os números aleatorios
    for (int i = 0; i < N; i++) {
        unsigned long int random_value = rand();
		printf("%lu ", random_value);
		primos += ehPrimo(random_value);
    }
	
	printf("%lu", primos);
	
	return 0;
}
