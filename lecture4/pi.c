#include <stdio.h>
#include <math.h>
#include <mpi.h>

#define ROUNDS 5
/* */
double PI25DT = 3.141592653589793238462643;

int main( int argc, char *argv[] ){   
   
   MPI_Init(&argc,&argv);
   int n, myid, numprocs, i,j; 
   double mypi, pi, h, sum, x; 
   
   MPI_Comm_rank(MPI_COMM_WORLD,&myid);
   MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
 
   /* we perform multiple rounds, with an increasing n */
   for(j=0;j<ROUNDS;++j){
       /* emulate user input */
       if(myid == 0){
           n = pow(10, j+1);
       }

       /*************************/
       /* YOUR TASK STARTS HERE */
       /*************************/
	
       // Broadcast 'n' from process 0
       MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

       // Compute local 'mypi'
       h = 1.0 / (double) n; 
       sum = 0.0; 
       for (i = myid + 1; i <= n; i += numprocs) { 
           x = h * ((double)i - 0.5); 
           sum += 4.0 / (1.0 + x*x); 
       } 
       mypi = h * sum; 
      
       // Reduce local 'mypi' to process 0
       MPI_Reduce(&mypi, &pi, 1, MPI_DOUBLE, MPI_SUM, 0,MPI_COMM_WORLD);

       /***********************/
       /* YOUR TASK ENDS HERE */
       /***********************/
       
       if (myid == 0) 
         printf("pi is approx: %.16f, Error is %.16f\n",pi, fabs(pi- PI25DT));
   } 
   
   MPI_Finalize(); 
   return 0; 
}
