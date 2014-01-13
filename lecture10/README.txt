I. THIS FOLDER CONTAINS:
omp_nBody.c: the source code of the OpenMP implementation of a 2-dimensional n-body solver with contains 4 implementions of:
+ Serial basic algorithm
+ OpenMP (omp) basic algorithm
+ Serial reduced algorithm
+ OpenMP (omp) reduced algorithm (with a cyclic partitioning of threads)

Note: in the function of the OpenMP reduced algorithm (omp_update_system_reduced), 
in the for loop to compute the local force
the scheduler is set to (static,1) to perform a cyclic partitioning of threads
One can change the scheduler to (static) to perform the block schedule.

II. COMPILE
gcc -o omp_nBody omp_nBody.c -lm -fopenmp

III. COMMAND LINE ARGUMENTS:
-n (int): the number of particles
-m (int): the number of step
-s (double): the step size
-t (int): the number of threads 
-d (int): optinal argument, to display the detail of the system or not

IV. EXAMPLES:
1. With 400 particles, 1000 steps, the size of each step is 1.0 second, 4 threads,
do not display the initial and the detail of the system after each step:
./omp_nBody -n 400 -m 1000 -s 1.0 -t 4
nBody solver with: 
400 particles, 1000 step, step size = 1.000000 (s)
number of threads = 4
====  Perform Serial Basic Algorithm  ====
Done
====   Perform Parallel Basic Algorithm  ====
Done
====  Perform Serial Reduced Algorithm  ====
Done
====   Perform Parallel Reduced Algorithm  ====
Done
====
Max Differnces:
Serial Basic - Omp Basic: pos(0.000000,0.000000),vel(0.000000,0.000000)
Serial Basic - Serial Reduced: pos(0.000000,0.000000),vel(0.000000,0.000000)
Serial Basic - Omp Reduced: pos(0.000000,0.000000),vel(0.000000,0.000000)
Running time:
Serial, Basic algorithm	: 3.570524 (s)
OpenMP, Basic algorithm	: 0.914076 (s), speed-up = 3.906158
Serial, Reduced algorithm	: 1.829326 (s)
OpenMP, Reduced algorithm	: 0.483879 (s), speed-up = 3.780543
====
Speed-up between the OpenMP Reduced and the Serial Basic algorithm: 7.378957 


2. With 4 particles, 2 steps, step size = 1.0 second, 4 steps
display the detail of the system after each step:
./omp_nBody -n 4 -m 2 -s 1.0 -t 4 -d
nBody solver with: 
4 particles, 2 step, step size = 1.000000 (s)
number of threads = 4
Initial state of the system:
particle 0: mass = 5.000000e+24, pos = (0.000000e+00,0.000000e+00), vel = (0.000000e+00,0.000000e+00)
particle 1: mass = 5.000000e+24, pos = (1.000000e+02,1.000000e+02), vel = (3.000000e+01,3.000000e+01)
particle 2: mass = 5.000000e+24, pos = (2.000000e+02,2.000000e+02), vel = (6.000000e+01,6.000000e+01)
particle 3: mass = 5.000000e+24, pos = (3.000000e+02,3.000000e+02), vel = (9.000000e+01,9.000000e+01)

====  Perform Serial Basic Algorithm  ====
After step 1
particle 0: pos = (0.000000e+00,0.000000e+00), vel = (1.605609e+10,1.605609e+10)
particle 1: pos = (1.300000e+02,1.300000e+02), vel = (2.949077e+09,2.949077e+09)
particle 2: pos = (2.600000e+02,2.600000e+02), vel = (-2.949077e+09,-2.949077e+09)
particle 3: pos = (3.900000e+02,3.900000e+02), vel = (-1.605609e+10,-1.605609e+10)
After step 2
particle 0: pos = (1.605609e+10,1.605609e+10), vel = (2.555673e+10,2.555673e+10)
particle 1: pos = (2.949077e+09,2.949077e+09), vel = (4.694093e+09,4.694093e+09)
particle 2: pos = (-2.949077e+09,-2.949077e+09), vel = (-4.694093e+09,-4.694093e+09)
particle 3: pos = (-1.605609e+10,-1.605609e+10), vel = (-2.555673e+10,-2.555673e+10)
Done
====   Perform Parallel Basic Algorithm  ====
After step 1
particle 0: pos = (0.000000e+00,0.000000e+00), vel = (1.605609e+10,1.605609e+10)
particle 1: pos = (1.300000e+02,1.300000e+02), vel = (2.949077e+09,2.949077e+09)
particle 2: pos = (2.600000e+02,2.600000e+02), vel = (-2.949077e+09,-2.949077e+09)
particle 3: pos = (3.900000e+02,3.900000e+02), vel = (-1.605609e+10,-1.605609e+10)
After step 2
particle 0: pos = (1.605609e+10,1.605609e+10), vel = (2.555673e+10,2.555673e+10)
particle 1: pos = (2.949077e+09,2.949077e+09), vel = (4.694093e+09,4.694093e+09)
particle 2: pos = (-2.949077e+09,-2.949077e+09), vel = (-4.694093e+09,-4.694093e+09)
particle 3: pos = (-1.605609e+10,-1.605609e+10), vel = (-2.555673e+10,-2.555673e+10)
Done
====  Perform Serial Reduced Algorithm  ====
After step 1
particle 0: pos = (0.000000e+00,0.000000e+00), vel = (1.605609e+10,1.605609e+10)
particle 1: pos = (1.300000e+02,1.300000e+02), vel = (2.949077e+09,2.949077e+09)
particle 2: pos = (2.600000e+02,2.600000e+02), vel = (-2.949077e+09,-2.949077e+09)
particle 3: pos = (3.900000e+02,3.900000e+02), vel = (-1.605609e+10,-1.605609e+10)
After step 2
particle 0: pos = (1.605609e+10,1.605609e+10), vel = (2.555673e+10,2.555673e+10)
particle 1: pos = (2.949077e+09,2.949077e+09), vel = (4.694093e+09,4.694093e+09)
particle 2: pos = (-2.949077e+09,-2.949077e+09), vel = (-4.694093e+09,-4.694093e+09)
particle 3: pos = (-1.605609e+10,-1.605609e+10), vel = (-2.555673e+10,-2.555673e+10)
Done
====   Perform Parallel Reduced Algorithm  ====
After step 1
particle 0: pos = (0.000000e+00,0.000000e+00), vel = (1.605609e+10,1.605609e+10)
particle 1: pos = (1.300000e+02,1.300000e+02), vel = (2.949077e+09,2.949077e+09)
particle 2: pos = (2.600000e+02,2.600000e+02), vel = (-2.949077e+09,-2.949077e+09)
particle 3: pos = (3.900000e+02,3.900000e+02), vel = (-1.605609e+10,-1.605609e+10)
After step 2
particle 0: pos = (1.605609e+10,1.605609e+10), vel = (2.555673e+10,2.555673e+10)
particle 1: pos = (2.949077e+09,2.949077e+09), vel = (4.694093e+09,4.694093e+09)
particle 2: pos = (-2.949077e+09,-2.949077e+09), vel = (-4.694093e+09,-4.694093e+09)
particle 3: pos = (-1.605609e+10,-1.605609e+10), vel = (-2.555673e+10,-2.555673e+10)
Done
====
Max Differnces:
Serial Basic - Omp Basic: pos(0.000000,0.000000),vel(0.000000,0.000000)
Serial Basic - Serial Reduced: pos(0.000000,0.000000),vel(0.000000,0.000000)
Serial Basic - Omp Reduced: pos(0.000000,0.000000),vel(0.000000,0.000000)
Running time:
Serial, Basic algorithm	: 0.000071 (s)
OpenMP, Basic algorithm	: 0.000244 (s), speed-up = 0.288915
Serial, Reduced algorithm	: 0.000067 (s)
OpenMP, Reduced algorithm	: 0.000088 (s), speed-up = 0.761361
====
Speed-up between the OpenMP Reduced and the Serial Basic algorithm: 0.798927

