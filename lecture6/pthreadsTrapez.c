#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

/* The global variables are shared among all the threads. */
int    thread_count = 4;
int    n = 10000;
double a = 0.0;
double b = 10.0;
double h;
int    local_n;

/* critical section lock variables */
int 	flag;
sem_t 	sem;
pthread_mutex_t mutex;

/* global result */
double	total_flag = 0.0;
double  total_sem = 0.0;
double 	total_mutex = 0.0;

void *Thread_work(void* rank);
/* Calculate local integral  */
double Trap(double local_a, double local_b, int local_n, double h);
/* function we're integrating */
double f(double x); 


int main(int argc, char** argv) {
    int i;
    int  thread_args[thread_count];
    pthread_t*  thread_handles;
    h = (b-a)/n;
    local_n = n/thread_count;

    /* Allocate storage for thread handles. */
    thread_handles = malloc (thread_count*sizeof(pthread_t));
	
    /* Initialize busy-waiting, mutex, semaphore */
    flag = 0;
    pthread_mutex_init(&mutex, NULL);
    sem_init(&sem, 0, 1);
	
    /* Start the threads. */
    for (i = 0; i < thread_count; i++) {
	thread_args[i] = i;
        pthread_create(&thread_handles[i], NULL, Thread_work, (void*) &thread_args[i]);
    }
	
    /* Wait for threads to complete. */
    for (i = 0; i < thread_count; i++) {
        pthread_join(thread_handles[i], NULL);
    }
    
    printf("The function f(x) = x² , in the interval [%.4f, %.4f], has an estimate area of:\n", a,b);
    printf("busy waiting: %f\n", total_flag);
    printf("mutex: %f\n", total_mutex);
    printf("semaphore: %f\n", total_sem);
	
    pthread_mutex_destroy(&mutex);
    sem_destroy(&sem);
    free(thread_handles);
	
    return 0;
}

/* thread programm */
void *Thread_work(void* rank) {
    double      local_a;   /* Left endpoint my thread   */
    double      local_b;   /* Right endpoint my thread  */
    double      my_int;    /* Integral over my interval */
    int         my_rank = *((long*) rank);
	
    /* Length of each process' interval of integration = */
    /* local_n*h.  So my interval starts at:             */
    local_a = a + my_rank*local_n*h;
    local_b = local_a + local_n*h;
	
    my_int = Trap(local_a, local_b, local_n, h);

    /************************/
    /* YOUR TASK START HERE */
    /************************/

    // (1) implement 3 different ways for critical sections
    // (2) for each way, use a different 'total' variable
    //	   total_flag  == busy waiting
    // 	   total_mutex == mutex
    //     total_sem   == semaphore

    /* busy waiting */ 
    while(flag != my_rank);
    total_flag += my_int;
    flag = (flag+1) % thread_count;

    /* semaphore */
    sem_wait(&sem);
    total_sem += my_int;
    sem_post(&sem);

    /* mutex */
    pthread_mutex_lock(&mutex);
    total_mutex += my_int;
    pthread_mutex_unlock(&mutex);

    /**********************/
    /* YOUR TASK END HERE */
    /**********************/
    
    return NULL;
}  

/*  Trap function */
double Trap(
	     double  local_a   /* in */,
	     double  local_b   /* in */,
	     int     local_n   /* in */,
	     double  h         /* in */) {
    /* Store result in integral  */	
    double integral;   
    double x;
    int i;
	
    integral = (f(local_a) + f(local_b))/2.0;
    x = local_a;
    for (i = 1; i <= local_n-1; i++) {
        x = local_a + i*h;
        integral += f(x);
    }
    integral = integral*h;
    return integral;
}

/* f(x) = x² */
double f(double x) {
    double return_val;
	
    return_val = x*x;
    return return_val;
}
