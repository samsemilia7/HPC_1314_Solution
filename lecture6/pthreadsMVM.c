#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
 
#define NUM_THREADS 4
#define DIM 16

//matrix
int A[DIM][DIM];
//vector
int x[DIM];
//result
int y[DIM];

void *MVM(void *argument)
{
	int tid,i,j;
	tid = *((int *) argument);
   	
	//distribute work blockwise
	int block = DIM/NUM_THREADS;
	for(i=tid*block;i<(tid+1)*block;++i){
		y[i] = 0;
		for(j=0;j<DIM;++j){
			y[i] += A[i][j]*x[j];
		}
	}
 	
	return NULL;
}
 
int main(void){

	pthread_t threads[NUM_THREADS];
	int thread_args[NUM_THREADS];
	int i,j;
	/* read input */
	FILE* input;
		input =	fopen("mvm.txt", "r");
		if(input == NULL) printf("Error reading Input\n");
		for(i=0;i<DIM;++i){
			for(j=0;j<DIM;++j){
				fscanf(input, "%d", &A[i][j]);
			}
			x[i] = i+1;
		}
	/* create all threads */
	for (i=0; i<NUM_THREADS; ++i) {
		thread_args[i] = i;
		pthread_create(&threads[i], NULL, MVM, (void *) &thread_args[i]);
	}

	/* wait for all threads to complete */
	for (i=0; i<NUM_THREADS; ++i) {
		pthread_join(threads[i], NULL);
	}

	for(i=0;i<DIM;++i){
		printf("%d\n", y[i]);
	}

	return 0;
}
