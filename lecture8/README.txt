I. THIS FOLDER CONTAINS:
1. sortOpenMP.c: the source code of the "Odd Even Transposition Sorting algorithm", with the serial implementation and 2 OpenMP implementations.
2. 50KInt.dat: input data, which contains 50K integers. The value of each integer is from 0 to 999.

II. COMPILE
gcc -o sortOpenMP sortOpenMP.c -fopenmp

III. COMMAND LINE ARGUMENTS:
-i: input data
-s: data size
-n: thread number
-d: optinal argument, to display the input and sorted result or not.

IV. EXAMPLES:

1. Run with small data, display the input and sorted result:
./sortOpenMP -i 50KInt.dat -s 16 -n 4 -d
Thread number: 4
There are 51200 integers in 50KInt.dat
Successfuly read 16 integer from file
Sorting with the serial algorithm
Done
Sorting with the OpenMP algorithm, Method 1
Done
Sorting with the OpenMP algorithm, Method 2
Done
Input data:
672	869	204	2	164	970	617	701	880	937	890	879	257	231	373	996	
Serial Odd-Even Transposition sort:
2	164	204	231	257	373	617	672	701	869	879	880	890	937	970	996	
OpenMP Odd-Even Transposition sort (method 1):
2	164	204	231	257	373	617	672	701	869	879	880	890	937	970	996	
OpenMP Odd-Even Transposition sort (method 2):
2	164	204	231	257	373	617	672	701	869	879	880	890	937	970	996	
Comparing the sorting result, between the serial method with the OpenMP Method 1: correct!
Comparing the sorting result, between the serial method with the OpenMP Method 2: correct!
Running time:
Serial	: 0.000004 (s)
OpenMP Method1	: 0.000132 (s), speed-up = 0.030886
OpenMP Method2	: 0.000012 (s), speed-up = 0.342409


2. Run with 20K integers:
./sortOpenMP -i 50KInt.dat -s 20000 -n 4 
Thread number: 4
There are 51200 integers in 50KInt.dat
Successfuly read 20000 integer from file
Sorting with the serial algorithm
Done
Sorting with the OpenMP algorithm, Method 1
Done
Sorting with the OpenMP algorithm, Method 2
Done
Comparing the sorting result, between the serial method with the OpenMP Method 1: correct!
Comparing the sorting result, between the serial method with the OpenMP Method 2: correct!
Running time:
Serial	: 0.789658 (s)
OpenMP Method1	: 0.542201 (s), speed-up = 1.456392
OpenMP Method2	: 0.250590 (s), speed-up = 3.151196

