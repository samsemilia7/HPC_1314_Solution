/*
  odd-even transposition sort by using OpenMP
   The sorting algorithm is prallelized by using OpenMP 
   with 2 methods:
   + Method 1: Use 2 "parallel for" directives
   + Method 2: Uses 1 "parallel for" directive and 2 "for" directives
*/
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <getopt.h>
#include <string.h>
/**/
/*global variables*/
char* strInputFileName; /* input data file name */
int dataSize;           /* input data size */
int threadNumber;       /* thread number */
int* dataArray;         /* input data array */
int isDisplay = 0;      /* by default, 
			   do not display the input data 
			   and the sorted data. 
			   One can change this variable to adding: "-d" in the 
			   input argument of the program to display 
			   both the input data and the sorted results.
			*/
/**/
/* To parse the input arguments of the application */
void parseArgs(int argc, char** argv);
/* Serial odd-even transposition sort */
void odd_even_sort_serial(int* a, int n);
/* openmp implementation, method 1*/
void odd_even_sort_openmp_method1(int*a, int n);
/* openmp implementation, method 2*/
void odd_even_sort_openmp_method2(int*a, int n);
/* to read the input data*/
void read_data_input();
/* to compare the sorted results*/
int compareArray(int* arr1, int* arr2, int n);
/* to display the data*/
void display_results(int* dataArray1, int* dataArray2, int* dataArray3, int n);
/*
  main function
  ./sortOpenMP -i input_data_file_name -s input_data_size -n thread_number [-d] 
  -d is an optinal arguments
  example: sort 20000 integers in the file 50KInt.dat with 8 threads
  ./sortOpenMP -i 50KInt.dat -s 20000 -n 8
  ./sortOpenMP -i 50KInt.dat -s 20000 -n 8 -d
*/
int main(int argc, char** argv)
{
  int i;
  int* dataArray1, *dataArray2, *dataArray3;
  double startTimeSerial, endTimeSerial, elapsedTimeSerial;
  double startTimeOmp1, endTimeOmp1, elapsedTimeOmp1;
  double startTimeOmp2, endTimeOmp2, elapsedTimeOmp2;
  /**/
  parseArgs(argc,argv);
  /*read data from the input file*/
  printf("Thread number: %d\n",threadNumber);
  /**/
  read_data_input();
  /*copy dataArray to dataArray2, dataArray3, to perform the sort algorithms*/
  dataArray1 = (int*) malloc(sizeof(int)*dataSize);
  dataArray2 = (int*) malloc(sizeof(int)*dataSize);
  dataArray3 = (int*) malloc(sizeof(int)*dataSize);
  /**/
  memcpy(dataArray1,dataArray,sizeof(int)*dataSize);
  memcpy(dataArray2,dataArray,sizeof(int)*dataSize);
  memcpy(dataArray3,dataArray,sizeof(int)*dataSize);
  /**/
  /*if isDisplay = 1, display the input and the sorted data*/
  /*sort*/
  printf("Sorting with the serial algorithm\n");
  startTimeSerial = omp_get_wtime();
  odd_even_sort_serial(dataArray1,dataSize);
  endTimeSerial = omp_get_wtime();
  elapsedTimeSerial = endTimeSerial - startTimeSerial;
  printf("Done\n");
  /**/
  printf("Sorting with the OpenMP algorithm, Method 1\n");
  startTimeOmp1 = omp_get_wtime();
  odd_even_sort_openmp_method1(dataArray2,dataSize);
  endTimeOmp1 = omp_get_wtime();
  elapsedTimeOmp1 = endTimeOmp1 - startTimeOmp1;
  printf("Done\n");
  /**/
  printf("Sorting with the OpenMP algorithm, Method 2\n");
  startTimeOmp2 = omp_get_wtime();
  odd_even_sort_openmp_method2(dataArray3,dataSize);
  endTimeOmp2 = omp_get_wtime();
  elapsedTimeOmp2 = endTimeOmp2 - startTimeOmp2;
  printf("Done\n");
  /**/
  if(isDisplay == 1)
  {
    display_results(dataArray1,dataArray2,dataArray3,dataSize);
  }
  /* verify the results */
  printf("Comparing the sorting result, between the serial method with the OpenMP Method 1: ");
  if(compareArray(dataArray1,dataArray2,dataSize)) printf("correct!\n");
  else(printf("incorrect!"));
  /**/
  printf("Comparing the sorting result, between the serial method with the OpenMP Method 2: ");
  if(compareArray(dataArray1,dataArray3,dataSize)) printf("correct!\n");
  else(printf("incorrect!"));
  /* running time report */
  printf("Running time:\n");
  printf("Serial\t: %lf (s)\n",elapsedTimeSerial);
  printf("OpenMP Method1\t: %lf (s), speed-up = %lf\n",elapsedTimeOmp1,elapsedTimeSerial/elapsedTimeOmp1);
  printf("OpenMP Method2\t: %lf (s), speed-up = %lf\n",elapsedTimeOmp2,elapsedTimeSerial/elapsedTimeOmp2);
  /* free the dynamic allocations*/
  free(strInputFileName);
  free(dataArray);
  free(dataArray1);
  free(dataArray2);
  free(dataArray3);
  /**/
  return 0;
}
/**/
void read_data_input()
{
  FILE* fInput;
  size_t fileSize;
  /**/
  fInput = fopen(strInputFileName,"r");
  /*If there is a problem in opening file, exit the program*/
  if(fInput == NULL)
  {
    printf("Can not open the input file\n");
    exit(1);
  }
  /*calculate the number of integers in the input file*/
  fseek(fInput,0,SEEK_END);
  fileSize = ftell(fInput)/sizeof(int);
  printf("There are %zu integers in %s\n",fileSize,strInputFileName);
  if(fileSize < dataSize) dataSize = fileSize;
  /*allocate dataArray and read data from the input file*/
  dataArray = (int*) malloc(sizeof(int)*dataSize);
  fseek(fInput,0,SEEK_SET);
  fread(dataArray,sizeof(int),dataSize,fInput);
  fclose(fInput);
  /**/
  printf("Successfuly read %d integer from file\n",dataSize);
}
/**/
void odd_even_sort_serial(int* a, int n)
{
  int phase, i, temp;
  for(phase=0;phase<n;++phase)
  {
    if(phase%2==0) //even phase
    {
      for(i=1;i<n;i+=2)
	if(a[i-1] > a[i])
	{
	  temp = a[i];
	  a[i] = a[i-1];
	  a[i-1] = temp;
	}
    }
    else //odd phase
    {
      for(i=1;i<n-1;i+=2)
	if(a[i] > a[i+1])
	{
      	  temp = a[i];
	  a[i] = a[i+1];
	  a[i+1] = temp;
	}
    }
  }
}
/**/
void odd_even_sort_openmp_method1(int* a, int n)
{
  /* 
     Method 1: Use 2 "parallel for" directives
  */
  int phase, i, temp;
  for(phase=0;phase<n;++phase)
  {
    if(phase%2==0) //even phase
    {
#pragma omp parallel for num_threads(threadNumber) default(none) shared(a,n) private(i,temp)
      for(i=1;i<n;i+=2)
	if(a[i-1] > a[i])
	{
	  temp = a[i];
	  a[i] = a[i-1];
	  a[i-1] = temp;
	}
    }
    else //odd phase
    {
#pragma omp parallel for num_threads(threadNumber) default(none) shared(a,n) private(i,temp)
      for(i=1;i<n-1;i+=2)
	if(a[i] > a[i+1])
	{
      	  temp = a[i];
	  a[i] = a[i+1];
	  a[i+1] = temp;
	}
    }
  }
}
/**/
void odd_even_sort_openmp_method2(int* a, int n)
{
  /*
    Method 2: Uses 1 "parallel for" directive and 2 "for" directives
  */
  int phase, i, temp;
#pragma omp parallel num_threads(threadNumber) default(none) shared(a,n) private(i,temp,phase)
  for(phase=0;phase<n;++phase)
  {
    if(phase%2==0) //even phase
    {
#pragma omp for
      for(i=1;i<n;i+=2)
	if(a[i-1] > a[i])
	{
	  temp = a[i];
	  a[i] = a[i-1];
	  a[i-1] = temp;
	}
    }
    else //odd phase
    {
#pragma omp for
      for(i=1;i<n-1;i+=2)
	if(a[i] > a[i+1])
	{
      	  temp = a[i];
	  a[i] = a[i+1];
	  a[i+1] = temp;
	}
    }
  }
}
/**/
void display_results(int* dataArray1, int* dataArray2, int* dataArray3, int n)
{
  int i;
  /**/
  printf("Input data:\n");
  for(i=0;i<n;++i) printf("%d\t",dataArray[i]);
  printf("\n");
  /**/
  printf("Serial Odd-Even Transposition sort:\n");
  for(i=0;i<n;++i) printf("%d\t",dataArray1[i]);
  printf("\n");
  /**/
  printf("OpenMP Odd-Even Transposition sort (method 1):\n");
  for(i=0;i<n;++i) printf("%d\t",dataArray2[i]);
  printf("\n");
  /**/
  printf("OpenMP Odd-Even Transposition sort (method 2):\n");
  for(i=0;i<n;++i) printf("%d\t",dataArray3[i]);
  printf("\n");
}
/**/
int compareArray(int* arr1, int* arr2, int n)
{
  int isEqual = 1;
  int i = 0;
  /**/
  while((isEqual==1)&&(i<n))
  {
    if(arr1[i] == arr2[i]) ++i;
    else(isEqual = 0);
  }
  /**/
  return isEqual;
}
/**/
void parseArgs(int argc, char** argv)
{
  char c;
  int optionIndex = 0;
  struct option longOption[]=
  {
    {"input-file",1,NULL,'i'},
    {"number",1,NULL,'n'},
    {"is-display",1,NULL,'d'},
    {"thread-number",1,NULL,'n'},
    {0,0,0,0}
  };
  if (argc < 7) 
  {
    printf("Wrong number of arguments\n");
    exit(1);
  }
  while((c=getopt_long(argc,argv,"n:i:s:d",longOption,&optionIndex))!=-1)
  {
    switch(c)
    {
      case 'i':
	strInputFileName = strdup(optarg);
	break;
      case 's':
	dataSize = atoi(optarg);
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
