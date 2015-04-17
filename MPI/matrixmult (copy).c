#define N 512
#include <stdio.h>
#include <sys/time.h>
#include "mpi.h"

void print_results(char *prompt, double a[N][N]) {
	int i, j;
	printf ("\n\n%s\n", prompt);
	for (i = 0; i< N; i++) {
		for (j = 0; j < N; j++) {
			printf(" %.2lf", a[i][j]);
		}
		printf ("\n");
	}
	printf ("\n\n");
}

int main(int argc, char *argv[])	{
	int i, j, k, error = 0;
	double a[N][N], b[N][N], c[N][N];

	
	char *usage = "Usage: %s file\n";
	FILE *fd;

	double elapsed_time;
	struct timeval tv1, tv2;

	MPI_Status status; // MPI variables
	int rank, P, blksz;

	MPI_Init(&argc, &argv);  		// Start MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &P);

	if ((rank == 0) && (N % P != 0)) {
		error = -1;
		printf("Error -- N/P must be an integer\n");
	} else blksz = N/P;


	if(rank == 0) { // open file
		if (argc< 2) {
			fprintf (stderr, usage, argv[0]);
			error = -1;
		}
		if ((fd = fopen (argv[1], "r")) == NULL) {
			fprintf (stderr, "%s: Cannot open file %s for reading.\n",argv[0], argv[1]);
			fprintf (stderr, usage, argv[0]);
			error = -1;
		}
	}

	if(rank == 0) {  // broadcast any error and close down if error
		for (i = 1 ; i < P; i++) {
			MPI_Send(&error, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
		}
	} else {
		MPI_Recv(&error, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status); 
	}

	if (error != 0) {  //terminate the process
		printf("This is process %d... An error occurred...I am shutting down....\n",rank);
		MPI_Finalize();
	return 0;
	}

	if (rank == 0) { // read file
		for (i = 0; i< N; i++)
			for (j = 0; j < N; j++)
				fscanf (fd, "%lf", &a[i][j]);
		for (i = 0; i< N; i++)
			for (j = 0; j < N; j++)
				fscanf (fd, "%lf", &b[i][j]);
	}

	MPI_Barrier(MPI_COMM_WORLD); // Add a barrier prior to the time stamp.

	if (rank == 0) gettimeofday(&tv1, NULL);


	MPI_Scatter(a, blksz*N, MPI_DOUBLE, a, blksz*N, MPI_DOUBLE, 0, MPI_COMM_WORLD);// Scatter input matrix a
	MPI_Bcast(b, N*N, MPI_DOUBLE, 0, MPI_COMM_WORLD); // Broadcast the input matrix b

	for(i = 0 ; i < blksz; i++) {
		for(j = 0 ; j < N ; j++) {
			c[i][j] = 0;
			for(k = 0 ; k < N ; k++) {
				c[i][j] += a[i][k] * b[k][j];
			}
		}
	}

	MPI_Gather(c, blksz*N, MPI_DOUBLE, c, blksz*N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	if(rank == 0) {
		gettimeofday(&tv2, NULL);
		elapsed_time = (tv2.tv_sec - tv1.tv_sec) + ((tv2.tv_usec - tv1.tv_usec) / 1000000.0);
		printf ("elapsed_time=\t%lf (seconds)\n", elapsed_time);
		//print_results("C = ", c); // Used to check output
	}
	MPI_Finalize(); 
	return 0;
}
