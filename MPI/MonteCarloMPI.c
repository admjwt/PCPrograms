#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <mpi.h>
#define USE_MPI   
#define SEED 35791246
#define PI 3.14159265358979

main(int argc, char *argv[])
{
   int niter=0;
   int nSeed = 0;
   double x,y;
   int i,j,count=0,mycount; /* # of points in the 1st quadrant of unit circle */
   double z;
   double pi;
   int myid,numprocs,proc;
   MPI_Status status;
   int master =0;
   int tag = 123;
   numprocs = 4;
   MPI_Init(&argc,&argv);
   MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
   MPI_Comm_rank(MPI_COMM_WORLD,&myid);
   
   printf("Enter the number of iterations used to estimate pi: ");
   scanf("%d",&niter); 	
   
   printf("Enter the number of seeds to send: ");
   scanf("%d",&nSeed); 


     /* initialize random numbers */
   double start, finish;
   start=MPI_Wtime();
   srand(SEED);
   mycount=0;
	for(i=0; i< nSeed; i++){
		if (myid ==0) { /* if I am the master process gather results from others */
      		count = mycount;
      		for (proc=1; proc<numprocs; proc++) {
        	 MPI_Recv(&mycount,1,MPI_REAL,proc,tag,MPI_COMM_WORLD,&status);
        	 count +=mycount;        
      		}
      		pi=(double)count/(niter*numprocs)*4;
      		printf("\n # of trials= %d , estimate of pi is %g \n",niter*numprocs,pi);
    
   		}
   		else { /* for all the slave processes send results to the master */
		for ( i=0; i<niter; i++) {
     		x = (double)rand()/RAND_MAX;
      		y = (double)rand()/RAND_MAX;
      		z = x*x+y*y;
     		 if (z<=1) mycount++; 
   		}
     		 printf("Processor %d sending results= %d to master process\n",myid,mycount
      		);
      		MPI_Send(&mycount,1,MPI_REAL,master,tag,MPI_COMM_WORLD);
   		}
	}
   finish=MPI_Wtime();
   MPI_Finalize(); /* let MPI finish up */

   printf("Parallel Elapsed time: %f seconds\n", finish-start); 
   double error = PI - pi;
   printf("Amount of error: \t%lf \n",error); 
             
  
}
