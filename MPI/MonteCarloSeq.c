#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <sys/time.h>
#include "mpi.h"
#define SEED 35791246
#define PI 3.14159265358979

main(int argc, char* argv)
{
   int niter=0;
   double x,y;
   int i,count=0; /* # of points in the 1st quadrant of unit circle */
   double z;
   double pi;

   printf("Enter the number of iterations used to estimate pi: ");
   scanf("%d",&niter);

   /* initialize random numbers */
   struct timeval tv1, tv2;
   gettimeofday(&tv1, NULL);
   srand(SEED);
   count=0;
   for ( i=0; i<niter; i++) {
      x = (double)rand()/RAND_MAX;
      y = (double)rand()/RAND_MAX;
      z = x*x+y*y;
      if (z<=1) count++;
      }
   pi=(double)count/niter*4;

   gettimeofday(&tv2, NULL);
   double elapsed_time = (tv2.tv_sec - tv1.tv_sec) + 
   ((tv2.tv_usec - tv1.tv_usec) / 1000000.0);

   printf("# of trials= %d , estimate of pi is %g \n",niter,pi);
   printf("Elapsed Time: \t%lf (seconds)\n", elapsed_time);
   double error = PI - pi;
   printf("Amount of error: \t%lf \n",error);
   
}
