#include <stdio.h> 
#include <cuda.h> 
#include <stdlib.h> 
#include <time.h>

//matrix multiplication done by the device
__global__ void DevMM(int *d_a,int *d_b, int *d_c, int N) { 
	
 	int row = blockIdx.y * blockDim.y + threadIdx.y; 
 	int col = blockIdx.x * blockDim.x + threadIdx.x;
	int sum = 0;
	if( col < N && row < N) {
		for(int i = 0; i < N; i++) {
			sum += d_a[row * N + i] * d_b[i * N + col];
		}
		d_c[row * N + col] = sum;
	}
} 

//the matrix multiplcation done by the host
void HostMM(int * h_a, int* h_b, int* h_c, int N) {
	int row, col, k, sum;

	for(row = 0; row < N; row++) {
		for(col = 0; col < N; col++) {
			sum = 0;
			for(k = 0; k < N; k++) {
				sum += h_a[row * N + k] * h_b[k * N + col];
			}
			h_c[row * N + col] = sum;
		}
	}
}
//gets the usr intputs for the size of the array, number of threads and number of blocks
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

int main(void) { 
	
	int N, B, T;				
	int *a, *b, *c, *d; 			
	int *dev_a, *dev_b, *dev_c; 
	int size;				
	cudaEvent_t start, stop;	// Time measuring events
	float etD, etH;
	cudaEventCreate(&start);
	cudaEventCreate(&stop);

	// Get array dimensions from the user
	getN(&N);
	getB(&B);
	getT(&T);

	// Grid and block structures
	dim3 Grid(B, B); 	//amount blocks per grid in the x and y directions
	dim3 Block(T, T); 	//amount of threads per block in x and y idrections

	// Total size of arrays in bytes
	size = N * N * sizeof(int);

	// Set size of arrays
	a = (int*) malloc(size);
	b = (int*) malloc(size);
	c = (int*) malloc(size);
	d = (int*) malloc(size);

	// Initialize matrices with random numbers and intalize c and d to zero
	srand(1);
	for (int i = 0; i < N; i++) { //i is the row j is column
		for(int j = 0 ; j < N; j++) {
			
			int valA = rand() % 10;
			a[i * N + j] = valA;
			int valB = rand() % 10;
			b[i * N + j] = valB;
			c[i * N + j] = 0;
			d[i * N + j] = 0;
		}
	} 
	
	// Allocate memory on the device
	cudaMalloc((void**)&dev_a, N * N * sizeof(int)); 
	cudaMalloc((void**)&dev_b, N * N * sizeof(int)); 
	cudaMalloc((void**)&dev_c, N * N * sizeof(int)); 

	// Copy the array from the host to the device
	cudaMemcpy(dev_a, a , N * N * sizeof(int),cudaMemcpyHostToDevice); 
	cudaMemcpy(dev_b, b , N * N * sizeof(int),cudaMemcpyHostToDevice); 
	cudaMemcpy(dev_c, c , N * N * sizeof(int),cudaMemcpyHostToDevice); 
	// Start timing the cuda computation
	cudaEventRecord(start, 0);

	DevMM<<<Grid, Block>>>(dev_a, dev_b, dev_c, N);
	cudaMemcpy(c, dev_c, size, cudaMemcpyDeviceToHost);

	// Measure end of compuatation
	cudaEventRecord(stop, 0);
	cudaEventSynchronize(stop);
	cudaEventElapsedTime(&etD, start, stop);

	printf("Time spent by Device: %f ms.\n", etD);

	// Host computation
	cudaEventRecord(start, 0);

	HostMM(a, b, d, N);
	
	// Measure end of compuatation
	cudaEventRecord(stop, 0);
	cudaEventSynchronize(stop);
	cudaEventElapsedTime(&etH, start, stop);
	printf("Time spent by Host: %f ms.\n", etH);
	printf("Speedup Factor: %f\n",(etH/etD));
	//check if matrices are the same
	for(int i = 0; i < N; i++) {
		for(int j = 0; j < N; j++) {
			if(c[i * N + j] != d[i * N + j]){
				printf("Matrices are not the same\n");
				exit(0);
			}	
		}
	}
	//}
	free(a); 
	free(b); 
	free(c); 
	free(d);
	cudaFree(dev_a); 
	cudaFree(dev_b); 
	cudaFree(dev_c); 
	cudaEventDestroy(start);
	cudaEventDestroy(stop);

	return 0; 
}
