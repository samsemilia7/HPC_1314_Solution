#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define N 10
#define THREADS 4

void Schedule(int n);

/* main */
int main(int argc, char* argv[]) {
    Schedule(N);
    return 0;
}  
void Schedule(int n){
	int liste[n];
	int i,j;
	/************************/
	/*YOUR TASK STARTS HERE */
	/************************/
	/*
		1. please write a program which determine the shedule type,
		   you should fill 'liste' with the thread id's.
		2. set the schedule attribute to 'runtime'
        3. and use num_threads(THREADS)
		4. for a hint look at the print function
	*/
	#pragma omp parallel num_threads(THREADS) shared(liste,n) default(none)
    {
		int myid = omp_get_thread_num();
		#pragma omp for schedule(runtime)
		for(i=0;i<n;++i){
			liste[i] = myid;
		}

	}
	
	/************************/
	/*YOUR TASK ENDS HERE */
	/************************/

	/* print results */
	for(i=0;i<THREADS;++i){
		printf("thread[%d] = {", i);
		for(j=0;j<n;++j){
			if(liste[j] == i){
				printf("%d, ", j);
			}
		}
		printf("}\n");
	}
}