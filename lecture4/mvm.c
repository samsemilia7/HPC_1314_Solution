#include <stdio.h>
#include <mpi.h>

#define SIZE 1024

int main(int argc, char* argv[]){
	
	MPI_Init(&argc, &argv);
	int myid, numprocs;
	int result[SIZE];
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	if(numprocs > SIZE){ MPI_Finalize(); return 0; }
	/* init matrix A and vector x */
	int A[SIZE][SIZE];
	int x[SIZE];
	int i,j;
	if(myid == 0){
		for(i=0;i<SIZE;++i){
			for(j=0;j<SIZE;++j){
				A[i][j] = (i*SIZE+j) % 3;
			}
			x[i] = (SIZE-i) % 3;
		}
	}
	/*************************/
	/* YOUR TASK STARTS HERE */	
	/*************************/

	//distribute data
	int block = SIZE/numprocs;
	int myA[block][SIZE];

	MPI_Bcast(x, SIZE, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Scatter(A, block*SIZE, MPI_INT,myA, block*SIZE, MPI_INT, 0, MPI_COMM_WORLD);
		
	//calculate local values
	int myY[block];
	for(i=0;i<block;++i){
		myY[i] = 0;
		for(j=0;j<SIZE;++j){
			myY[i] += myA[i][j]*x[j];
		}
	}	
	//collect data
	MPI_Gather(myY, block, MPI_INT, result, block, MPI_INT, 0 , MPI_COMM_WORLD);
	
	/***********************/
	/* YOUR TASK ENDS HERE */
	/***********************/

	/* print result vector */	
	if(myid == 0){
		for(i=0;i<SIZE;++i){
			printf("%d ", result[i]);
		}
		printf("\n");
	}

	MPI_Finalize(); 
	return 0;
}
