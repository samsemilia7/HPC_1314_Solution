I. THIS FOLDER CONTAINS:

omp_back_substitution.c: the source code of the Backward Substitution step in the Gaussian Elimination for solving systems of linear equations:
  Ax = b
This step is parallelized with OpenMP, using the row-oriented algorthm and the column-oriented algorithm.

As this is only the Backward Substitution step, the input is the "upper triangular linear system" (the result of the "row operations" step).
This system is simulated in the applications (more details can be found in the function: init_input_data()) 

II. COMPILE
gcc -o omp_back_substitution omp_back_substitution.c -fopenmp -lm

III. COMMAND LINE ARGUMENTS:
-s: system size
-n: thread number
-d: optinal argument, to display the input and the result or not.

IV. EXAMPLES:
1. With small system, display the input and the result:
./omp_back_substitution -n 4 -s 8 -d
thread number = 4
system size = 8
initializing input data:
done
====
Matrix A:
0.800  8.222  7.000  8.375  16.000  6.000  21.250  36.000  
0.000  0.800  8.750  25.000  16.333  13.833  3.000  2.667  
0.000  0.000  0.800  4.833  30.000  11.000  0.429  73.000  
0.000  0.000  0.000  0.800  3.889  19.500  7.600  25.000  
0.000  0.000  0.000  0.000  0.800  13.800  11.750  11.400  
0.000  0.000  0.000  0.000  0.000  0.800  10.000  93.000  
0.000  0.000  0.000  0.000  0.000  0.000  0.800  3.500  
0.000  0.000  0.000  0.000  0.000  0.000  0.000  0.800  
Vector b:
103.647  70.383  120.062  56.789  37.750  103.800  4.300  0.800  
Backward substitution with the serial row-oriented algorithm:
Done
Backward substitution with the OpenMP row-oriented algorithm:
Done
Backward substitution with the serial column-oriented algorithm:
Done
Backward substitution with the OpenMP column-oriented algorithm:
Done
====
result by using the row-oriented method, serial:
1.000  1.000  1.000  1.000  1.000  1.000  1.000  1.000  
result by using the row-oriented method, openmp:
1.000  1.000  1.000  1.000  1.000  1.000  1.000  1.000  
result by using the column-oriented method, serial:
1.000  1.000  1.000  1.000  1.000  1.000  1.000  1.000  
result by using the column-oriented method, openmp:
1.000  1.000  1.000  1.000  1.000  1.000  1.000  1.000  
Max error in the results of the OpenMP row-oriented algorithm:	 0.000000e+00
Max error in the results of the OpenMP column-oriented algorithm:	 0.000000e+00
====
Running time:
Serial, row-oriented	: 0.000002 (s)
OpenMP, row-oriented	: 0.000131 (s), speed-up = 0.016185
Serial, column-oriented	: 0.000001 (s)
OpenMP, column-oriented	: 0.000014 (s), speed-up = 0.063348

2. With a system of size 10000, 4 threads:
./omp_back_substitution -n 4 -s 10000 
thread number = 4
system size = 10000
initializing input data:
done
====
Backward substitution with the serial row-oriented algorithm:
Done
Backward substitution with the OpenMP row-oriented algorithm:
Done
Backward substitution with the serial column-oriented algorithm:
Done
Backward substitution with the OpenMP column-oriented algorithm:
Done
====
Max error in the results of the OpenMP row-oriented algorithm:	 0.000000e+00
Max error in the results of the OpenMP column-oriented algorithm:	 0.000000e+00
====
Running time:
Serial, row-oriented	: 0.150396 (s)
OpenMP, row-oriented	: 0.060764 (s), speed-up = 2.475077
Serial, column-oriented	: 0.611580 (s)
OpenMP, column-oriented	: 0.227509 (s), speed-up = 2.688163
