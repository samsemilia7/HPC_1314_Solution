#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#define SIZE 16
#define THRESHOLD 0.001 

/*
 * nice print of a matrix
 * @param matrix Pointer to matrix
 * @param dim_x Number of rows
 *
 * example call:
 * printMatrix(jacobi,5);
 */
void printMatrix(float matrix[][SIZE], int dim_x);

int main(int argc, char* argv[]){
    
    	MPI_Init(&argc, &argv);
    	int myid, numprocs;
	int i,j;
	
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	
	float jacobi[SIZE][SIZE];
	/* handle input */
	if(myid == 0){
		FILE* input;
		input =	fopen("jacobiInput.txt", "r");
		if(input == NULL) printf("Error reading Input\n");
		for(i=0;i<SIZE;++i){
			for(j=0;j<SIZE;++j){
				fscanf(input, "%f", &jacobi[i][j]);
			}
		}
	}
	/*
	 * suppose input is a power of two and we distribute the matrix blockwise
	 */
	int block = SIZE/numprocs;
	/*
	 * data type myJacobi
	 * dim_x: block+2, so we have 2 extra rows for receiving
	 */
	float myJacobi[block+2][SIZE];
	float myNewJacobi[block+2][SIZE];
	/*
	 * distribute data among processes
	 */
	if(myid == 0){
		/*
 		 * master process stores receiving elements at position 0, because it only needs to receive the last row
  		 */
		MPI_Scatter(jacobi, block*SIZE, MPI_FLOAT, &myJacobi[0], block*SIZE,MPI_FLOAT, 0, MPI_COMM_WORLD);
	}else{
		/*		
 		 * other processes store receiving elements at position 1, because they need to receive the first and last row
		 * (execpt for the last process, he only needs to to receive the first row, but we handle him as the others)
  		 */
		MPI_Scatter(jacobi, block*SIZE, MPI_FLOAT, &myJacobi[1], block*SIZE,MPI_FLOAT, 0, MPI_COMM_WORLD);
	}
	
	int convergence = 1;
	
	while(convergence){
		

		/*************************/
	        /* YOUR TASK STARTS HERE */
                /*************************/

		// we perform 2 non-blocking sends
		MPI_Request send_request[2];
		//we perform 2 non-blocking recvs
		MPI_Request recv_request[2];
    		MPI_Status status;
		// 1. use non-blocking send/recv 
		// 2. remember master/last process only need to send/recv one row
		// 3. don't handle MPI_Wait
		
		//Phase 1
		if(myid<numprocs-1){
			if(myid==0){
				MPI_Isend(myJacobi[block-1], SIZE, MPI_FLOAT, myid+1, 17, MPI_COMM_WORLD, &send_request[0]);
			}else{
				MPI_Isend(myJacobi[block], SIZE, MPI_FLOAT, myid+1, 17, MPI_COMM_WORLD, &send_request[0]);
				MPI_Irecv(myJacobi[0], SIZE, MPI_FLOAT, myid-1, 17, MPI_COMM_WORLD, &recv_request[0]);
			}			
		}else{
			MPI_Irecv(myJacobi[0], SIZE, MPI_FLOAT, myid-1, 17, MPI_COMM_WORLD, &recv_request[0]);
		}		
		
		//Phase 2
		if(myid>0){
			MPI_Isend(myJacobi[1], SIZE, MPI_FLOAT, myid-1, 23, MPI_COMM_WORLD, &send_request[1]);
			if(myid <numprocs-1){
				MPI_Irecv(myJacobi[block+1], SIZE, MPI_FLOAT, myid+1, 23, MPI_COMM_WORLD, &recv_request[1]);
			}
		}else{
			MPI_Irecv(myJacobi[block], SIZE, MPI_FLOAT, myid+1, 23, MPI_COMM_WORLD, &recv_request[1]);
		}
		/*************************/
	        /* YOUR TASK ENDS HERE */
                /*************************/
	
		int rows;
		if(myid > 0 && myid < numprocs -1){
			rows = block+1;
		}else{
			rows = block;
		}
		float diffnorm = 0.0;
		/*
		 * calculating independent values and convergence
		 */
		for(i=2;i<rows-1;++i){
			for(j=1;j<SIZE-1;++j){
				myNewJacobi[i][j] = (myJacobi[i-1][j]+myJacobi[i+1][j]+myJacobi[i][j-1]+myJacobi[i][j+1])/4.0;
				diffnorm += (myNewJacobi[i][j] - myJacobi[i][j])*(myNewJacobi[i][j] - myJacobi[i][j]);
			}	
		}
		/*
		 * waiting for first row
		 */
		if(myid<numprocs-1){
			MPI_Wait(&send_request[0], &status);
		}
		if(myid>0){
			MPI_Wait(&recv_request[0], &status);
		}
		
		i=1;
		for(j=1;j<SIZE-1;++j){
			myNewJacobi[i][j] = (myJacobi[i-1][j]+myJacobi[i+1][j]+myJacobi[i][j-1]+myJacobi[i][j+1])/4.0;
			diffnorm += (myNewJacobi[i][j] - myJacobi[i][j])*(myNewJacobi[i][j] - myJacobi[i][j]);
		}	
		/*
		 * waiting for last row
		 */
		if(myid>0){
			MPI_Wait(&send_request[1], &status);
		}
		if(myid<numprocs-1){
			MPI_Wait(&recv_request[1], &status);
		}

		i=rows-1;
		for(j=1;j<SIZE-1;++j){
			myNewJacobi[i][j] = (myJacobi[i-1][j]+myJacobi[i+1][j]+myJacobi[i][j-1]+myJacobi[i][j+1])/4.0;
			diffnorm += (myNewJacobi[i][j] - myJacobi[i][j])*(myNewJacobi[i][j] - myJacobi[i][j]);
		}

		diffnorm = sqrt(diffnorm);
		for(i=1;i<rows;++i){
			for(j=1;j<SIZE-1;++j){
				myJacobi[i][j] = myNewJacobi[i][j];
			}	
		}
		/* 
		 * checking for convergence  
		 */
		float diffnormReduce;
		MPI_Reduce(&diffnorm, &diffnormReduce, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);

		if(myid == 0){
			if(diffnormReduce < THRESHOLD){
				convergence = 0;
				MPI_Bcast(&convergence, 1, MPI_INT,0,MPI_COMM_WORLD);
			}else{
				convergence = 1;
				MPI_Bcast(&convergence, 1, MPI_INT,0,MPI_COMM_WORLD);
			}
		}else{
			MPI_Bcast(&convergence, 1, MPI_INT, 0, MPI_COMM_WORLD);
		}
	
	}
	/*
	 * gather output for sauce
	 */
	if(myid == 0){
		MPI_Gather(myJacobi[0], block*SIZE, MPI_FLOAT, jacobi, block*SIZE, MPI_FLOAT, 0, MPI_COMM_WORLD);
	}else{
		MPI_Gather(myJacobi[1], block*SIZE, MPI_FLOAT, jacobi, block*SIZE, MPI_FLOAT, 0, MPI_COMM_WORLD);
	}
	if(myid == 0){
		printMatrix(jacobi, SIZE);
	}
	MPI_Finalize();
	return 0;
}

void printMatrix(float matrix[][SIZE], int dim_x){
	int i,j;
	for(i=0;i<dim_x;++i){
		for(j=0;j<SIZE;++j){
			printf("%.4f ", matrix[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}
