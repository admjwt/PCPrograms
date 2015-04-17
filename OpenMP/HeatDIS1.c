#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

#define TOTAL_TIME 30000
#define N 900
#define M 900

double computeValue (double A[ ][M], int i, int j)
{
	return (A[i-1][j] + A[i+1][j] + A[i][j-1] + A[i][j+1]) * 0.25;
}
int main (int argc, char *argv[]) {
	int __guin_rank = 0;
	int __guin_current = 0;
	int __guin_next = 1;

	{
 int i, j,n, m, max_iterations;
 int left, right;
 double room[2][N][M];
 double elapsed_time, start_time, end_time;
 struct timeval tv1, tv2;
 left = (int) round(M * 0.3);
 right = M - left;

 for (j = 0; j < N; j++) {
 if (j >= left && j <= right)
 room[0][0][j] = room[1][0][j] = 100.0;
 else
 room[0][0][j] = room[1][0][j] = 0.0;
 }

 for (i = 1; i < N; i++) {
 for (j = 0; j < N; j++) {
 room[0][i][j] = room[1][i][j] = 0.0;
 }
 }

 gettimeofday(&tv1, NULL);

 ;
 #pragma paraguin begin_parallel

 n = N;
 m = M;
 max_iterations = TOTAL_TIME;

 #pragma paraguin stencil room n m max_iterations computeValue

 #pragma paraguin end_parallel

 gettimeofday(&tv2, NULL);

 elapsed_time = (tv2.tv_sec - tv1.tv_sec) +
 ((tv2.tv_usec - tv1.tv_usec) / 1000000.0);
	printf ("elapsed_time=nt%lf (seconds)nn", elapsed_time);

 for (i = 0; i < N; i++) {
 for (j = 0; j < N; j++) {
 printf (" %lf", room[__guin_current][i][j]);
 }
 printf ("nn");
 }
printf ("nnnn");	
return(0);
}

