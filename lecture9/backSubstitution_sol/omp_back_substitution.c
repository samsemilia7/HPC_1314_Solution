/*
  Paralllelism the Backward Substitution step
  in the Gaussian Elimination
  for solving systems of linear equations:
  Ax = b
  with OpenMP, 
  using the row-oriented algorthm 
  and column-oriented algorithm.
  ====
  
*/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<omp.h>
#include <getopt.h>
#include <math.h>
#include<time.h>
/**/
/*global variables*/
int systemSize;           /* the number of linear equations in the system */
int threadNumber = 1;       /* thread number, =1 by default */
double* A;                 /* matrix A */
double* b;                 /* vector b */ 
int isDisplay = 0;      /* by default, 
			   do not display the input data 
			   and the result. 
			   One can change this variable to adding: "-d" in the 
			   input argument of the program to display 
			   both the input data and the results.
			*/
/* To parse the input arguments of the application */
void parseArgs(int argc, char** argv);
/* 
   init input data
   randomly generate the upper-right triangle of the matrix A
   assume that all the elements in the vector X is 1.0
   calculate the element in the vector b
   => the results after solving this equations should be 
   a vector which contains all 1.0
 */
void init_input_data();
/* display input data: matrix A and vector b*/
void display_input_data();
/* display result */
void display_result(double* x_row_serial, double* x_row_omp, double* x_col_serial, double* x_col_omp);
/* 
   finding the max error in the result
   as we know the result vector should contain all 1.0
 */
double find_error(double* a);
/* serial implementation of the row-oriented backward substitution */
void serial_row_oriented_back_substitution(double* A,double* b, double* x);
/* serial implementation of the column-oriented backward substitution */
void serial_col_oriented_back_substitution(double* A,double* b, double* x);
/* openmp implementation of the row-oriented backward substitution */
void omp_row_oriented_back_substitution(double* A,double* b, double* x);
/* openmp implementation of the column-oriented backward substitution */
void omp_col_oriented_back_substitution(double* A,double* b, double* x);
/*
  main function
  1. To compile:
  gcc -o omp_back_substitution omp_back_substitution.c -fopenmp -lm
  2. To run:
  ./omp_back_substitution  -s system_size -n thread_number [-d] 
  -d is an optinal arguments
  example: for the system of size 10000 with 4 threads
  ./omp_back_substitution -s 10000 -n 4
  ./omp_back_substitution -s 10000 -n 8 -d
*/
int main(int argc, char** argv)
{
  double *x_row_serial, *x_col_serial, *x_row_omp, *x_col_omp; 
  double startTimeSerial1, endTimeSerial1, elapsedTimeSerial1;
  double startTimeSerial2, endTimeSerial2, elapsedTimeSerial2;
  double startTimeOmp1, endTimeOmp1, elapsedTimeOmp1;
  double startTimeOmp2, endTimeOmp2, elapsedTimeOmp2;
  double diff1, diff2;
  /* parse the input arguments from the command line */
  parseArgs(argc,argv);
  /**/
  printf("thread number = %d\n",threadNumber);
  printf("system size = %d\n",systemSize);
  /**/
  init_input_data();
  /**/
  if(isDisplay) display_input_data();
  /* allocate the result vectors */
  x_row_serial = (double*) malloc(sizeof(double)*systemSize);
  x_col_serial = (double*) malloc(sizeof(double)*systemSize);
  x_row_omp = (double*) malloc(sizeof(double)*systemSize);
  x_col_omp = (double*) malloc(sizeof(double)*systemSize);
  /* row oriented method, serial */
  printf("Backward substitution with the serial row-oriented algorithm:\n");
  startTimeSerial1 = omp_get_wtime();
  serial_row_oriented_back_substitution(A,b,x_row_serial);
  endTimeSerial1 = omp_get_wtime();
  elapsedTimeSerial1 = endTimeSerial1 - startTimeSerial1;
  printf("Done\n");
  /* row oriented method, OpenMP */
  printf("Backward substitution with the OpenMP row-oriented algorithm:\n");
  startTimeOmp1 = omp_get_wtime();
  omp_row_oriented_back_substitution(A,b,x_row_omp);
  endTimeOmp1 = omp_get_wtime();
  elapsedTimeOmp1 = endTimeOmp1 - startTimeOmp1;
  printf("Done\n");
  /* column-oriented method, serial */
  printf("Backward substitution with the serial column-oriented algorithm:\n");
  startTimeSerial2 = omp_get_wtime();
  serial_col_oriented_back_substitution(A,b,x_col_serial);
  endTimeSerial2 = omp_get_wtime();
  elapsedTimeSerial2 = endTimeSerial2 - startTimeSerial2;
  printf("Done\n");
  /* column oriented method, OpenMP */
  printf("Backward substitution with the OpenMP column-oriented algorithm:\n");
  startTimeOmp2 = omp_get_wtime();
  omp_col_oriented_back_substitution(A,b,x_col_omp);
  endTimeOmp2 = omp_get_wtime();
  elapsedTimeOmp2 = endTimeOmp2 - startTimeOmp2;
  printf("Done\n");
  printf("====\n");
  /* */
  if(isDisplay) display_result(x_row_serial,x_row_omp,x_col_serial,x_col_omp);
  /* find error */
  diff1 = find_error(x_row_omp);
  diff2 = find_error(x_col_omp);
  /**/
  printf("Max error in the results of the OpenMP row-oriented algorithm:\t %e\n",diff1);
  printf("Max error in the results of the OpenMP column-oriented algorithm:\t %e\n",diff2);
  printf("====\n");
  /**/
  printf("Running time:\n");
  printf("Serial, row-oriented\t: %lf (s)\n",elapsedTimeSerial1);
  printf("OpenMP, row-oriented\t: %lf (s), speed-up = %lf\n",elapsedTimeOmp1,elapsedTimeSerial1/elapsedTimeOmp1);
  printf("Serial, column-oriented\t: %lf (s)\n",elapsedTimeSerial2);
  printf("OpenMP, column-oriented\t: %lf (s), speed-up = %lf\n",elapsedTimeOmp2,elapsedTimeSerial2/elapsedTimeOmp2);
  /* free allocated memory */
  free(A);
  free(b);
  free(x_row_serial);
  free(x_col_serial);
  free(x_row_omp);
  free(x_col_omp);
  /**/
  return 0;
}
/**/
void serial_row_oriented_back_substitution(double* A,double* b, double* x)
{
  int i,j;
  double tmp;
  /**/
  for(i=systemSize-1;i>=0;--i)
  {
    tmp = b[i];
    for(j=i+1;j<systemSize;++j) tmp -= A[i*systemSize+j]*x[j];
    x[i] = tmp/A[i*systemSize+i];
  }
}
/**/
void omp_row_oriented_back_substitution(double* A,double* b, double* x)
{
  /*
    1. The inner loop (with j) can be parallelized.
    2. The cumulative result of x[j] can be calculated by using the reduction clause
       => use the temporary value tmp
    3. The initialization and the final calculation of x[i] is performed by only 1 threads, 
       => we use the "omp single" directive.
  */
  int i,j;
  double tmp;
  /**/
  #pragma omp parallel num_threads(threadNumber) default(none) private(i,j) shared(A,b,x,tmp,systemSize)
  for(i=systemSize-1;i>=0;--i)
  {
    /*initiatlization of x[i]*/ 
    #pragma omp single
    tmp = b[i];
    /*
      Notice: you can specify different types of scheduling here
      to evaluate the performance
     */
    #pragma omp for reduction(+:tmp) schedule(static)
    for(j=i+1;j<systemSize;++j) tmp -= A[i*systemSize+j]*x[j];
    /* final calculation of x[i] */
    #pragma omp single 
    x[i] = tmp/A[i*systemSize+i];
  }
}
/**/
void serial_col_oriented_back_substitution(double* A,double* b, double* x)
{
  int i,j;
  /**/
  for(i=0;i<systemSize;++i) x[i] = b[i];
  /**/
  for(j=systemSize-1;j>=0;--j) 
  {
    x[j] /= A[j*systemSize+j];
    for(i=0;i<j;++i) x[i] -= A[i*systemSize+j]*x[j];
  }
}
/**/
void omp_col_oriented_back_substitution(double* A,double* b, double* x)
{
  /*
    Parallelize the column-oriented backward substitution algorithm
    1. The "INITIALIZATION LOOP" can be parallelized.
    2. In the "CALCULATION LOOP":
      2.1 The inner loop (with i) can be parallelized.
      2.2 We use the "omp single" directive for the final calculation of x[j] 
  */
  int i,j;
  /**/
  #pragma omp parallel num_threads(threadNumber) default(none) private(i,j) shared(A,b,x,systemSize)
  {
    /* INITIALIZATION LOOP */
    #pragma omp for
    for(i=0;i<systemSize;++i) x[i] = b[i];
    /**/
    /* CALCULATION LOOP */
    for(j=systemSize-1;j>=0;--j) 
    {
      /*final calculation of x[j]*/ 
      #pragma omp single 
      x[j] /= A[j*systemSize+j];
      /*
	Notice: you can specify different types of scheduling here
	to evaluate the performance
      */
      #pragma omp for schedule(static)
      for(i=0;i<j;++i) x[i] -= A[i*systemSize+j]*x[j];
    }
  }
}
/**/
/* init input data*/
void init_input_data()
{
  int i,j;
  int rand1, rand2;
  double* tmpX;
  /**/
  srand(time(NULL));
  /**/
  A = (double*) malloc(sizeof(double)*systemSize*systemSize);
  b = (double*) malloc(sizeof(double)*systemSize);
  tmpX = (double*) malloc(sizeof(double)*systemSize);
  /**/
  printf("initializing input data:\n");
  for(i=0;i<systemSize;++i)
  {
    for(j=0;j<i;++j) A[i*systemSize+j] = 0.0;
    A[i*systemSize+i] = systemSize/10.0;
    for(j=i+1;j<systemSize;++j)
    { 
      rand1 = rand()%100;
      rand2 = rand()%10;
      if(rand1 == 0) rand1 = 1;
      if(rand2 == 0) rand2 = 1;
      /**/
      A[i*systemSize+j] = (double) rand1/rand2;
    }
  }
  /**/
  for(i=0;i<systemSize;++i) tmpX[i] = 1.0;
  /**/
  for(i=0;i<systemSize;++i)
  {
    b[i] = 0.0;
    for(j=0;j<systemSize;++j) b[i] += A[i*systemSize+j]*tmpX[j];
  }
  printf("done\n");
  printf("====\n");
  free(tmpX);
}
/**/
void display_input_data()
{
  int i,j;
  /* display A */
  printf("Matrix A:\n");
  for(i=0;i<systemSize;++i)
  {
    for(j=0;j<systemSize;++j) printf("%.3lf  ",A[i*systemSize+j]);
    printf("\n");
  }
  /* display b*/
  printf("Vector b:\n");
  for(i=0;i<systemSize;++i) printf("%.3lf  ",b[i]);
  printf("\n");
}
/**/
void display_result(double* x_row_serial, double* x_row_omp, double* x_col_serial, double* x_col_omp)
{
  int i;
  /* display result*/
  printf("result by using the row-oriented method, serial:\n");
  for(i=0;i<systemSize;++i) printf("%.3lf  ",x_row_serial[i]);
  printf("\n");
  /**/
  printf("result by using the row-oriented method, openmp:\n");
  for(i=0;i<systemSize;++i) printf("%.3lf  ",x_row_omp[i]);
  printf("\n");
  /**/
  printf("result by using the column-oriented method, serial:\n");
  for(i=0;i<systemSize;++i) printf("%.3lf  ",x_row_serial[i]);
  printf("\n");
  /**/
  printf("result by using the column-oriented method, openmp:\n");
  for(i=0;i<systemSize;++i) printf("%.3lf  ",x_col_omp[i]);
  printf("\n");
  /**/
}
/* 
   finding the max error in the result
   as we know the result vector should contain all 1.0
 */
double find_error(double* a)
{
  int i;
  double returnDiff = 0.0;
  double diff;
  /**/
  for(i=0;i<systemSize;++i)
  { 
    diff = abs(a[i]-1.0);
    if(diff > returnDiff) returnDiff = diff;
  }
  /**/
  return returnDiff;
}
/* To parse the input arguments of the application */
void parseArgs(int argc, char** argv)
{
  char c;
  int optionIndex = 0;
  struct option longOption[]=
  {
    {"system-size",1,NULL,'s'},
    {"is-display",1,NULL,'d'},
    {"thread-number",1,NULL,'n'},
    {0,0,0,0}
  };
  if (argc < 5) 
  {
    printf("Wrong number of arguments\n");
    exit(1);
  }
  while((c=getopt_long(argc,argv,"n:s:d",longOption,&optionIndex))!=-1)
  {
    switch(c)
    {
      case 's':
	systemSize = atoi(optarg);
	break;
      case 'd':
	isDisplay = 1;
	break;
      case 'n':
	threadNumber = atoi(optarg);
	break;
      default:
	printf("Bad argument %c\n",c);
	exit(1);
    }
  }    
}
