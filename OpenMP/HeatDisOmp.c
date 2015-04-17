#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define CHUNKSIZE 512
#define N 100
int main (int argc, char *argv[]) {
	int current, next, iteration, MAX_ITERATIONS, i, j, k, temp, x;

	double start, end, sTime, pTime;			// used for timing
	
	printf("Please input an integer value for the amount of iterations: ");
   	scanf("%i",&MAX_ITERATIONS);
	
	float h[2][N][N];
	
	printf("\nSequential, not parallelized\n");
	for (x = 0; x < 2; x++) {
		for (j = 0; j < N; j++)
			for (k = 0; k < N; k++){
			h[x][j][k] = 20;
			if((x == 0 && j == 0 && k >= (N/10)*4 && k <= (N/10)*5)){
			h[x][j][k] = 100.0;
			}}
				
	}
	for(i=0; i < N; i += (N/10))
     		for(j=0; j < N; j += (N/10)) {
         		if (x%10 == 0){
				printf("\n");
			}
			x++;
         		printf("%7.2f",h[0][i][j]);
     	}
	printf("\n");
	
	current = 0;
	next = 1;
	start = omp_get_wtime();
	for (iteration = 0; iteration < MAX_ITERATIONS; iteration++) {
		for (i = 1; i < N-1; i++){
			for (j = 1; j < N-1; j++){
			h[next][i][j] = 0.25 * (h[current][i-1][j] + h[current][i+1][j]
			+ h[current][i][j-1] + h[current][i][j+1]);
			}
		}
	temp = current; // swap values of current and next
	current = next;
	next = temp;
	}
	end = omp_get_wtime();	//end time measurement
	sTime = end-start;
	for(i=0; i < N; i += (N/10)){
     		for(j=0; j < N; j += (N/10)) {
         		if (x%10 == 0){
				printf("\n");
			}
			x++;
         		printf("%7.2f",h[0][i][j]);
     		}
	}
	printf("\n");
	printf("Time of computation: %f seconds\n", end-start);

	printf("\nSequential, parallelized\n");
	
	float p[2][N][N];
	for (x = 0; x < 2; x++) {
		for (j = 0; j < N; j++)
			for (k = 0; k < N; k++){
			p[x][j][k] = 20;
			if((x == 0 && j == 0 && k >= (N/10)*4 && k <= (N/10)*5)){
			p[x][j][k] = 100.0;
			}}
				
	}
	for(i=0; i < N; i += (N/10))
     		for(j=0; j < N; j += (N/10)) {
         		if (x%10 == 0){
				printf("\n");
			}
			x++;
         		printf("%7.2f",p[0][i][j]);
     	}
	printf("\n");
	
	#pragma omp parallel private(i, j, current, next, temp, iteration)
	{
	start = omp_get_wtime();
	current = 0;
	next = 1;
	#pragma omp schedule(dynamic, CHUNKSIZE) for nowait
	for (iteration = 0; iteration < MAX_ITERATIONS; iteration++) {
		for (i = 1; i < N-1; i++){
			for (j = 1; j < N-1; j++){
			p[next][i][j] = 0.25 * (p[current][i-1][j] + p[current][i+1][j]
			+ p[current][i][j-1] + p[current][i][j+1]);
			}
		}
	temp = current; // swap values of current and next
	current = next;
	next = temp;
	}end = omp_get_wtime();	//end time measurement
}
	
	pTime = end-start;
	for(i=0; i < N; i += (N/10)){
     		for(j=0; j < N; j += (N/10)) {
         		if (x%10 == 0){
				printf("\n");
			}
			x++;
         		printf("%7.2f",p[0][i][j]);
     		}
	}
	printf("\n");
	printf("Time of parallel computation: %f seconds\n", end-start);
	
	int error = 0;
	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++)   {
			if (h[0][i][j] != p[0][i][j]) error = -1;
		}
	}
	if (error == -1) printf("ERROR, sequential and parallel versions give different answers\n");
	printf("Speed up factor: %f\n", sTime/pTime);
	return(0);
}

