/*
Name: Colton Hagan
Date : 3/16/21
Class : cs 347
Program : Final project, peforms dot multiplaction with threads
*/

#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>
#include <sys/time.h>

/* Matrices dimensions */
#define  Dim_X  200
#define  Dim_Y  300
#define  Dim_Z  200

#define Num_of_Threads 8

struct mm_thread_data {
	int start;
	int work;
	double *A;
	double *B;
	double *C;
};

/*The thread_fn function shall compute the data, i.e., dot product rows and columns, in the range of start to start+work. 
  The computed data shall be stored in matrix C.*/
void* thread_fn (void *arg) {
	struct mm_thread_data *thread_data = (struct mm_thread_data*) arg;
	pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
	double cell_result;
	for(int i = thread_data->start; i < thread_data->start + thread_data->work; i++) {
		cell_result = 0;
		int x = i % Dim_X;
		int y = i / Dim_X;
		for(int k = 0; k < Dim_Y; k++) {
			cell_result += thread_data->A[Dim_Y*x+k] * thread_data->B[Dim_Z*k+y];
		}
		pthread_mutex_lock(&mutex);
		thread_data->C[Dim_Z*x+y] = cell_result;
		pthread_mutex_unlock(&mutex);
	}
}

/*Create Num_of_Threads threads to multiply the matrices A and B.
  Compute start and size of work done by the thread. Each thread shall process equal(or almost equal) part  of the matrix multiplication.*/
void multiply_matrices(double *A, double *B, double *C) {
	struct mm_thread_data thread_data[Num_of_Threads];
	pthread_t threads[Num_of_Threads];
	int work = Dim_X * Dim_Z / Num_of_Threads;

	//creates and uses threads
	for(int i = 0; i < Num_of_Threads; i++) {
		struct mm_thread_data *data = &thread_data[i];
		data = malloc(sizeof(struct mm_thread_data));
		data->A = A;
		data->B = B;
		data->C = C;
		data->start = i*work;
		if(i == Num_of_Threads - 1) {
			data->work = (Dim_X*Dim_Z) - data->start;
		} else {
			data->work = work;
		}

		if(pthread_create(&threads[i], NULL, thread_fn, (void*) data) != 0) {
			printf("Error while creating threads\n");
		}
	}
	//joins threads for safty
	for(int i = 0; i < Num_of_Threads; i++) {
		void* res;
		pthread_join(threads[i], &res);
	}
}

/* Print a matrix of size x by y */
void print_matrix (double *A, int x, int y) {
	for(int i = 0; i < x; i++) {
		printf("Row %d:", i);
		for(int j = 0; j < y; j++) {
			printf("\t%f", A[y*i + j]);
		}
		printf("\n");
	}
}


/* Generate random data for a matrix of size x rows by y columns */
void generate_matrix (double *A, int x, int y) {
	for(int i = 0; i < x*y; i++) {
		double rand_value = 1000.0*(double)rand()/(double)RAND_MAX;
		A[i] = rand_value;
	}
}

/*The driver of the program.
 * Generate matrix A and matrix B using generate_matrix.
 * Multiply matrix A by matrix B using multiply_matrices.
 * Print matrix A, matrix B, and matrix C using print_matrix.
 * Compute and print the elapsed time in .3 precision.
 */
int main (int argc, char ** argv){
	struct timeval start_time;
	struct timeval end_time;
	// Allocate memory for the matrices A, B and C
	double* A = malloc(Dim_X * Dim_Y * sizeof(double*));
	double* B = malloc(Dim_Y * Dim_Z * sizeof(double));
	double* C = malloc(Dim_X * Dim_Z * sizeof(double));
	//Generate matrices A and B
	srand(time(NULL));
	generate_matrix(A, Dim_X, Dim_Y);
	generate_matrix(B, Dim_Y, Dim_Z);
	//Compute C as the result of A multiplied by B
	gettimeofday(&start_time, NULL);
	multiply_matrices(A,B,C);
	gettimeofday(&end_time, NULL);
	//print matrices
	printf ("-------------- orignal A matrix ------------------\n");
	print_matrix(A, Dim_X, Dim_Y);
	printf ("-------------- orignal B matrix ------------------\n");
	print_matrix(B, Dim_Y, Dim_Z);
	printf ("--------------  result C matrix ------------------\n");
	print_matrix(C, Dim_X, Dim_Z);
	//print the elapsed time
	printf("Elapsed time : %.3f seconds\n", (double) (end_time.tv_usec - start_time.tv_usec) / 1000000 + (double) (end_time.tv_sec - start_time.tv_sec));
	//free memory
	free(A);
	free(B);
	free(C);
	return 0;
}
