#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#define N 256

int main(int argc, char *argv) {
	omp_set_num_threads(1);//set number of threads here
	int i, j, k, x;
	double sum;
	double start, end;			// used for timing
	double A[N][N], B[N][N], C[N][N], D[N][N];	

	// set some initial values for A and B
	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++)     {
			A[i][j] = j*1;
			B[i][j] = i*j+2;
		}
	}


	// sequential matrix multiplication
	start = omp_get_wtime(); 	//start time measurement
	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++)   {
			sum = 0;
			for (k=0; k < N; k++) {
				sum += A[i][k]*B[k][j]; 
			}      
			C[i][j] = sum;
		}
	}
 	end = omp_get_wtime();	//end time measurement
	printf("Time of sequential computation: %f seconds\n", end-start);

	// parallel matrix multiplication where OpenMP directives will be added

	start = omp_get_wtime(); 	//start time measurement
	#pragma omp parallel for private(i, sum, j, k)
	for (i = 0; i < N; i++) {
		//#pragma omp parallel for private(j, k)
		for (j = 0; j < N; j++)   {
			sum = 0;
			for (k=0; k < N; k++) {
				sum += A[i][k]*B[k][j];
			}      
			D[i][j] = sum;
		}
	} 
	end = omp_get_wtime();	//end time measurement
	printf("Time of parallel computation: %f seconds\n", end-start);

	// check sequential and parallel versions give same answers
	int error = 0;
	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++)   {
			if (C[i][j] != D[i][j]) error = -1;
		}
	}
	if (error == -1) printf("ERROR, sequential and parallel versions give different answers\n");

	return(0);
}

