#include <stdio.h> 
#include <cuda.h> 
#include <stdlib.h> 
#include <time.h>

int getN(int* N) {
	printf("Please input size of array(NxN): ");
	scanf("%d", N);
	return 0;
}
int getB(int* B) {
	printf("Please input number of blocks(B): ");
	scanf("%d", B);
	return 0;
}
int getT(int* T) {
	printf("Please input number of threads(T): ");
	scanf("%d", T);
	return 0;
}
__global__ void devSum(int *output, int *input, int n) { 
	int i = blockIdx.x*blockDim.x + threadIdx.x;

    	if (blockIdx.x > 0)
        	output[i] += input[blockIdx.x - 1];
} 

int main(void){
	int N, B, T;
	//get user input for arraysize, number of blocks, and number of threads
	getN(&N);
	getB(&B);
	getT(&T);

	int *a, *b, *c, *dev_a, *dev_b;
	
	// Time measuring events
	cudaEvent_t start, stop;
	float etD, etH;
	cudaEventCreate(&start);
	cudaEventCreate(&stop);
	
	//Allocate memory on Device and Host
	a = (int*) malloc( sizeof(int) * N );
	b = (int*) malloc( sizeof(int) * N );
	c = (int*) malloc( sizeof(int) * N );
	cudaMalloc((void**)&dev_a,N * sizeof(int));
	cudaMalloc((void**)&dev_b,N * sizeof(int));

	srand(1);     //initialize random number generator 
	for (int i=0; i < N; i++){           // load array with some numbers 
		a[i] = rand() % 10; 
		//printf("%d ", a[i]);
		b[i] = 0;
	}
	//printf("\n");

	cudaEventRecord(start, 0); 	//Start Host time
	int sum =0;
	for(int i = 0; i < N; i++){
		//printf("%d ",a[i]);
		sum += a[i];
		b[i] = sum;
		
	}
	cudaEventRecord(stop, 0);	//End Host time and compute length of time
	cudaEventSynchronize(stop);
	cudaEventElapsedTime(&etH, start, stop);

	//Copy input array with random Integers and a Zero-ed array used for output
	cudaMemcpy(dev_a, a ,N * sizeof(int),cudaMemcpyHostToDevice);
	cudaMemcpy(dev_b, b ,N * sizeof(int),cudaMemcpyHostToDevice);

	// Start timing the Device computation
	cudaEventRecord(start, 0);

	devSum<<<B, T>>>(dev_b, dev_a, N);

	//Copy output array from Device back to the Host
	cudaMemcpy(c, dev_b, N * sizeof(int), cudaMemcpyDeviceToHost);
	
	// Measure end of compuatation
	cudaEventRecord(stop, 0);
	cudaEventSynchronize(stop);
	cudaEventElapsedTime(&etD, start, stop);

	//printf("\n");
	for(int i = 0; i < N; i++) {
			//printf("Dev: %d",c[i]);
			//printf(" host: %d\n",b[i]);

			if(c[i] != b[i]){//Check if Device and Host are different, if they are close the program
				printf("Results are not the same\n");
				exit(0);
			}	
		}
	printf("\nHost and Device produce the same results.");
	printf("\n");
	printf("Time spent by Device: %f ms.\n", etD);
	printf("Time spent by Host: %f ms.\n", etH);
	printf("Speedup Factor: %f\n",(etH/etD));
	cudaFree(dev_a);
	cudaFree(dev_b);
	
return 0;
}
