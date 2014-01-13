/*
  Using OpenMP to parallelize a 2-dimensional n-body solver 
  using the basic algorithm 
  and the reduced algorithm with a cyclic partitioning of threads
  1. Calculations:
  1.1 Forces: The force on particle i due to particle j is:
             -G (m_i*m_k) * (s_i - s_k)/|s_i - s_k|^3
  1.2 The position and the velocity of particle i at the time step (t+1) are:
      s_i(t+1) = s_i(t) + h * v_i(t)
      v_i(t+1) = v_i(t) + h * a_i(t)
      with h is the size of the time step (stepSize)
      a is the acceleretor, caused by the accumulated force on particle i
  2. To compile and to run the application:
  2.1 To compile:
  gcc -o omp_nBody omp_nBody.c -fopenmp -lm
  2.2 To run:  
  ./omp_nBody -n particle_number -m step_number -s step_size -t thread_number [-d]
  -d is an optimal argument (to display the data and the result after each step)
  example:
  ./omp_nBody -n 400 -m 1000 -s 1.0 -t 4
  ./omp_nBody -n 400 -m 1000 -s 1.0 -t 4 -d
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <getopt.h>
#include <omp.h>
/**/
/* constant */
#define DIM 2
#define X 0
#define Y 1
const double G = 6.673e-11; /* Gravitional constant */
/* global variables */
int nParticle; /* number of particles */
int nStep;     /* number of steps */
double stepSize;  /* step size */
int nThread;  /* number of threads */
int isDisplay = 0; /*by default, 
		     do not display the detail of the system after each step 
		     One can change this variable to adding: "-d" in the 
		     input argument of the program to display 
		     both the input data and the results.
		   */
/* data structures */
typedef double vect_t[DIM];
/* */
typedef struct _particle
{
  double mass; /* mass */
  vect_t pos;  /* position */
  vect_t vel;  /* velocity */
} particle;
/**/
/* To parse the input arguments of the application */
void parseArgs(int argc, char** argv);
/* to display the system after each step */
void printSystem(particle* sytem, int nParticle, int step);
/*   generate data for the solver */
void generate_data(particle* system, int nParticle);
/* 
   find the max difference between each pair of element in the array, 
   to verify if 2 arrays are similar
   this function is used to compaire the results of the algorithm
 */
particle find_max_difference(particle* system1, particle* system2, int nParticle);
/* compute force for the basic algorithm */
void compute_force_basic(int q, vect_t* forces, particle* system, int nParticle);
/* compute force for the reduced algorithm */
void compute_force_reduced(int q, vect_t* forces, particle* system, int nParticle);
/* update particle q in the system */
void update_particle(int q, particle* system, vect_t* forces);
/* serial update the whole system after each step */
void serial_update_system_basic(particle* system, int nParticle, int nStep, double stepSize);
/* serial update the whole system after each step */
void serial_update_system_reduced(particle* system, int nParticle, int nStep, double stepSize);
/* parallel update the whole system after each step */
void omp_update_system_basic(particle* system, int nParticle, int nStep, double stepSize);
/* parallel update the whole system after each step */
void omp_update_system_reduced(particle* system, int nParticle, int nStep, double stepSize);
/*
  Main function
  1. To compile:
  gcc -o omp_nBody omp_nBody.c -fopenmp -lm
  2 To run:  
  ./omp_nBody -n particle_number -m step_number -s step_size -t thread_number [-d]
  -d is an optimal argument (to display the data and the result after each step)
  example:
  ./omp_nBody -n 400 -m 1000 -s 1.0 -t 4
  ./omp_nBody -n 400 -m 1000 -s 1.0 -t 4 -d
*/
int main(int argc, char** argv)
{
  /* variables */
  FILE* fInput;
  particle* system;
  particle* serial_system_basic;
  particle* serial_system_reduced;
  particle* omp_system_basic;
  particle* omp_system_reduced;
  /**/
  double startTimeSerial1, endTimeSerial1, elapsedTimeSerial1;
  double startTimeSerial2, endTimeSerial2, elapsedTimeSerial2;
  double startTimeOmp1, endTimeOmp1, elapsedTimeOmp1;
  double startTimeOmp2, endTimeOmp2, elapsedTimeOmp2;
  /**/
  particle diff1, diff2, diff3;
  /* parse arguments */
  parseArgs(argc,argv);
  /**/
  printf("nBody solver with: \n");
  printf("%d particles, %d step, step size = %lf (s)\n",nParticle,nStep,stepSize);
  printf("number of threads = %d\n",nThread);
  /**/
  system = (particle*) malloc(sizeof(particle)*nParticle);
  serial_system_reduced = (particle*) malloc(sizeof(particle)*nParticle);
  omp_system_basic = (particle*) malloc(sizeof(particle)*nParticle);
  omp_system_reduced = (particle*) malloc(sizeof(particle)*nParticle);
  /* read data from file */
  generate_data(system,nParticle);
  /**/
  serial_system_basic = system;
  memcpy(serial_system_reduced,system,sizeof(particle)*nParticle);
  memcpy(omp_system_basic,system,sizeof(particle)*nParticle);
  memcpy(omp_system_reduced,system,sizeof(particle)*nParticle);
  /* serial update system with basic algorithm */
  printf("====  Perform Serial Basic Algorithm  ====\n");
  startTimeSerial1 = omp_get_wtime();
  serial_update_system_basic(serial_system_basic,nParticle,nStep,stepSize);
  endTimeSerial1 = omp_get_wtime();
  elapsedTimeSerial1 = endTimeSerial1 - startTimeSerial1;
  printf("Done\n");
  /*omp update system with basic algorithm */
  printf("====   Perform Parallel Basic Algorithm  ====\n");
  startTimeOmp1 = omp_get_wtime();
  omp_update_system_basic(omp_system_basic,nParticle,nStep,stepSize);
  endTimeOmp1 = omp_get_wtime();
  elapsedTimeOmp1 = endTimeOmp1 - startTimeOmp1;
  printf("Done\n");
  /* serial update system with reduced algorithm */
  printf("====  Perform Serial Reduced Algorithm  ====\n");
  startTimeSerial2 = omp_get_wtime();
  serial_update_system_reduced(serial_system_reduced,nParticle,nStep,stepSize);
  endTimeSerial2 = omp_get_wtime();
  elapsedTimeSerial2 = endTimeSerial2 - startTimeSerial2;
  printf("Done\n");
  /* omp update system with reduced algorithm */
  printf("====   Perform Parallel Reduced Algorithm  ====\n");
  startTimeOmp2 = omp_get_wtime();
  omp_update_system_reduced(omp_system_reduced,nParticle,nStep,stepSize);
  endTimeOmp2 = omp_get_wtime();
  elapsedTimeOmp2 = endTimeOmp2 - startTimeOmp2;
  printf("Done\n");
  printf("====\n");
  /* verify the result by comparing the other algorithm with 
   the basic serial algorithm */
  diff1 = find_max_difference(serial_system_basic,omp_system_basic,nParticle);
  diff2 = find_max_difference(serial_system_basic,serial_system_reduced,nParticle);
  diff3 = find_max_difference(serial_system_basic,omp_system_reduced,nParticle);
  /**/
  printf("Max Differnces:\n");
  printf("Serial Basic - Omp Basic: pos(%lf,%lf),vel(%lf,%lf)\n",diff1.pos[X],diff1.pos[Y],diff1.vel[X],diff1.vel[Y]);
  printf("Serial Basic - Serial Reduced: pos(%lf,%lf),vel(%lf,%lf)\n",diff2.pos[X],diff2.pos[Y],diff2.vel[X],diff2.vel[Y]);
  printf("Serial Basic - Omp Reduced: pos(%lf,%lf),vel(%lf,%lf)\n",diff3.pos[X],diff3.pos[Y],diff3.vel[X],diff3.vel[Y]);
  /**/
  printf("Running time:\n");
  printf("Serial, Basic algorithm\t: %lf (s)\n",elapsedTimeSerial1);
  printf("OpenMP, Basic algorithm\t: %lf (s), speed-up = %lf\n",elapsedTimeOmp1,elapsedTimeSerial1/elapsedTimeOmp1);
  printf("Serial, Reduced algorithm\t: %lf (s)\n",elapsedTimeSerial2);
  printf("OpenMP, Reduced algorithm\t: %lf (s), speed-up = %lf\n",elapsedTimeOmp2,elapsedTimeSerial2/elapsedTimeOmp2);
  printf("====\n");
  printf("Speed-up between the OpenMP Reduced and the Serial Basic algorithm: %lf \n", elapsedTimeSerial1/elapsedTimeOmp2);
  /**/
  free(system); 
  free(serial_system_reduced);
  free(omp_system_basic);
  free(omp_system_reduced);
  /* serial_system_basic is only assigned to system, don't need to free*/
  /**/
  return 0;
}
/**/
void printSystem(particle* system, int nParticle, int step)
{
  int i;
  printf("After step %d\n",step);
  for(i=0;i<nParticle;++i)
  {
    printf("particle %d: pos = (%e,%e), vel = (%e,%e)\n",i,system[i].pos[X],system[i].pos[Y],system[i].vel[X],system[i].vel[X]);
  }
}
/**/
void generate_data(particle* system, int nParticle)
{
  /*
    generate the system with n particles:
    + all particles have the same mass
    + the initial positions and velocities is of the particles are linear difference 
      (calculated by using the variable "gap")
  */
  double MASS = 5.0e24;
  double POS = 1.0e5;
  double VEL = 3.0e4;
  /**/
  double gap = 0.0;
  /**/
  int i;
  /**/
  for(i=0;i<nParticle;++i)
  {
    system[i].mass = MASS;
    /**/
    system[i].pos[X] = gap*POS;
    system[i].pos[Y] = gap*POS;
    system[i].vel[X] = gap*VEL;
    system[i].vel[Y] = gap*VEL;
    /**/
    gap += 0.001;
  }
  /**/
  if(isDisplay)
  {
    printf("Initial state of the system:\n");
    for(i=0;i<nParticle;++i)
      {
	printf("particle %d: mass = %e, pos = (%e,%e), vel = (%e,%e)\n",i,system[i].mass,system[i].pos[X],system[i].pos[Y],system[i].vel[X],system[i].vel[X]);
      }
    printf("\n");
  }
}
/* compute force for the basic algorithm */
void compute_force_basic(int q, vect_t* forces, particle* system, int nParticle)
{
  int k;
  double x_diff, y_diff, dist, dist_cubed;
  /**/
  forces[q][X] = forces[q][Y] = 0.0;
  for(k=0;k<nParticle;++k)
  {
    if(k != q)
    {
      /*calculate distance*/
      x_diff = system[q].pos[X] - system[k].pos[X];
      y_diff = system[q].pos[Y] - system[k].pos[Y];
      dist = sqrt(x_diff*x_diff + y_diff*y_diff);
      dist_cubed = dist*dist*dist;
      /*calculate force*/
      forces[q][X] -= (G * system[q].mass * system[k].mass * x_diff)/dist_cubed;
      forces[q][Y] -= (G * system[q].mass * system[k].mass * y_diff)/dist_cubed;
    }
  }
  /**/
}
/* compute force for the reduced algorithm */
void compute_force_reduced(int q, vect_t* forces, particle* system, int nParticle)
{
  int k;
  double x_diff, y_diff, dist, dist_cubed;
  vect_t force_qk;
  /**/
  for(k = q+1;k<nParticle;++k)
  {
    /*calculate distance*/
    x_diff = system[q].pos[X] - system[k].pos[X];
    y_diff = system[q].pos[Y] - system[k].pos[Y];
    dist = sqrt(x_diff*x_diff + y_diff*y_diff);
    dist_cubed = dist*dist*dist;
    /* calculate forces */
    force_qk[X] = -1.0*(G * system[q].mass * system[k].mass * x_diff)/dist_cubed;
    force_qk[Y] = -1.0*(G * system[q].mass * system[k].mass * y_diff)/dist_cubed;
    /* update total forces */
    forces[q][X] += force_qk[X];
    forces[q][Y] += force_qk[Y];
    forces[k][X] -= force_qk[X];
    forces[k][Y] -= force_qk[Y];
  }
}
/* update particle q in the system, based on the Euler method */
void update_particle(int q, particle* system, vect_t* forces)
{
  system[q].pos[X] += stepSize * system[q].vel[X];
  system[q].pos[Y] += stepSize * system[q].vel[Y];
  system[q].vel[X] += stepSize * (forces[q][X]/system[q].mass);
  system[q].vel[Y] += stepSize * (forces[q][Y]/system[q].mass);
}
/**/
void serial_update_system_basic(particle* system, int nParticle, int nStep, double stepSize)
{
  int step;
  vect_t* forces;
  int q;
  /**/
  forces = (vect_t*) malloc(sizeof(vect_t)*nParticle);
  for(step=1;step<=nStep;++step)
  {
    /* compute force */
    for(q=0;q<nParticle;++q)
      compute_force_basic(q,forces,system,nParticle);
    /* 
     update position and velocity of each particle 
     based on the Euler method
    */
    for(q=0;q<nParticle;++q)
      update_particle(q,system,forces);
    /* display */
    if(isDisplay)printSystem(system,nParticle,step);
  }
  /**/
  free(forces);
}
/**/
void omp_update_system_basic(particle* system, int nParticle, int nStep, double stepSize)
{
  int step;
  vect_t* forces;
  int q;
  double x_diff, y_diff, dist, dist_cubed;
  /**/
  forces = (vect_t*) malloc(sizeof(vect_t)*nParticle);
  #pragma omp parallel num_threads(nThread) default (none) \
    shared(system,forces,nThread,stepSize,nParticle,nStep,isDisplay)	\
    private (step,q)
  for(step=1;step<=nStep;++step)
  {
    /* calculate force */
    #pragma omp for 
    for(q=0;q<nParticle;++q)
      compute_force_basic(q,forces,system,nParticle);      
    /* 
       update position and velocity of each particle 
       based on the Euler method
    */
    #pragma omp for
    for(q=0;q<nParticle;++q)
      update_particle(q,system,forces);
    /* display */
    #pragma omp single
    if(isDisplay)
      printSystem(system,nParticle,step);
  }
  /**/
  free(forces);
}
/**/
/* serial update the whole system after each step */
void serial_update_system_reduced(particle* system, int nParticle, int nStep, double stepSize)
{
  int step;
  int q;
  vect_t* forces;
  /**/
  forces = (vect_t*) malloc(sizeof(vect_t)*nParticle);
  for(step=1;step<=nStep;++step)
  {
    for(q=0;q<nParticle;++q) forces[q][X] = forces[q][Y] = 0.0;
    /**/
    for(q=0;q<nParticle-1;++q)
      compute_force_reduced(q,forces,system,nParticle);
    /* 
       update position and velocity of each particle 
       based on the Euler method
    */
    for(q=0;q<nParticle;++q)
      update_particle(q,system,forces);
    /* display */
    if(isDisplay)
      printSystem(system,nParticle,step);
  }
  /**/
  free(forces);
}
/* parallel update the whole system after each step */
void omp_update_system_reduced(particle* system, int nParticle, int nStep, double stepSize)
{
  int step;
  int q;
  vect_t* forces;
  vect_t* local_forces;
  /**/
  forces = (vect_t*) malloc(sizeof(vect_t)*nParticle);
  local_forces = (vect_t*) malloc(sizeof(vect_t)*nParticle*nThread);
  /**/
#pragma omp parallel num_threads(nThread) default (none)\
  shared(system,forces,nThread,stepSize,nParticle,nStep,local_forces,isDisplay)	\
  private(step,q)
  {
    int my_rank = omp_get_thread_num();
    int thread;
    for(step=1;step<=nStep;++step)
    {
      #pragma omp for
      for(q=0;q<nParticle*nThread;++q) local_forces[q][X] = local_forces[q][Y] = 0.0;
      /* compute local force */
      /*
	The scheduler is set to (static,1) 
	to perform a cyclic partitioning of threads 
	If you want to perform the "block schedule" approach, 
	just set it to (static) 
      */ 
      #pragma omp for schedule(static,1)
      for(q=0;q<nParticle-1;++q)
	compute_force_reduced(q,local_forces+my_rank*nParticle,system,nParticle);
      /* compute the total force, the local force of each thread */
      #pragma omp for
      for(q=0;q<nParticle;++q)
      {
	forces[q][X] = forces[q][Y] = 0.0;
	for(thread=0;thread<nThread;++thread)
	{
	  forces[q][X] += local_forces[thread*nParticle+q][X];
	  forces[q][Y] += local_forces[thread*nParticle+q][Y];
	}
      }
      /* 
	 update position and velocity of each particle 
	 based on the Euler method
      */
      #pragma omp for
      for(q=0;q<nParticle;++q)
	update_particle(q,system,forces);
      /* display */
      #pragma omp single
      if(isDisplay)
	printSystem(system,nParticle,step);
    }
  }
  /**/
  free(forces);
  free(local_forces);
}
/* 
   find the max difference between each pair of element in the array, 
   to verify if 2 arrays are similar
   this function is used to compaire the results of the algorithm
*/
particle find_max_difference(particle* system1, particle* system2, int nParticle)
{
  particle returnValue;
  double tmp;
  int q;
  /**/
  returnValue.pos[X] = returnValue.pos[Y] = 0.0;
  returnValue.vel[X] = returnValue.vel[Y] = 0.0;
  /**/
  for(q=0;q<nParticle;++q)
  {
    tmp = abs(system1[q].vel[X] - system2[q].vel[X]);
    returnValue.vel[X] = (returnValue.vel[X] >= tmp) ? returnValue.vel[X] : tmp;
    /**/
    tmp = abs(system1[q].vel[Y] - system2[q].vel[Y]);
    returnValue.vel[Y] = (returnValue.vel[Y] >= tmp) ? returnValue.vel[Y] : tmp;
    /**/
    tmp = abs(system1[q].pos[X] - system2[q].pos[X]);
    returnValue.pos[X] = (returnValue.pos[X] >= tmp) ? returnValue.pos[X] : tmp;
    /**/
    tmp = abs(system1[q].pos[Y] - system2[q].pos[Y]);
    returnValue.pos[X] = (returnValue.pos[Y] >= tmp) ? returnValue.pos[Y] : tmp;
  }
  /**/
  return returnValue;
}
/**/
/* To parse the input arguments of the application */
void parseArgs(int argc, char** argv)
{
  char c;
  int optionIndex = 0;
  struct option longOption[]=
  {
    {"particle-number",1,NULL,'n'},
    {"step-number",1,NULL,'m'},
    {"step-size",1,NULL,'s'},
    {"thread-number",1,NULL,'t'},
    {"is-display",1,NULL,'d'},
    {0,0,0,0}
  };
  if (argc < 7) 
  {
    printf("Wrong number of arguments\n");
    exit(1);
  }
  while((c=getopt_long(argc,argv,"n:m:s:t:d",longOption,&optionIndex))!=-1)
  {
    switch(c)
    {
      case 'n':
	nParticle = atoi(optarg);
	break;
      case 'd':
	isDisplay = 1;
	break;
      case 'm':
	nStep = atoi(optarg);
	break;
      case 's':
	stepSize = strtod(optarg,NULL);
	break;
      case 't':
	nThread = atoi(optarg);
	break;
      default:
	printf("Bad argument %c\n",c);
	exit(1);
    }
  }    
}
