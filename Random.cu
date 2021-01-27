/*#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "VectorCuda.h"
#include "CudaStuff.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cuda.h>
#include <curand.h>

#include "RandomCuda.h"

__global__ 
KERNEL _MonteCarloTest(double* result,size_t len){
	size_t index = threadIdx.x + blockIdx.x*blockDim.x;
	if(index < len){
		result[index] = exp(result[index]) + 1;
	}
}

void dGenerateRandomVector(double** target, size_t len){
	double* temp;
	cudaMalloc(&temp,len*sizeof(double));

	curandGenerator_t generator;
	curandCreateGenerator(&generator, CURAND_RNG_PSEUDO_DEFAULT);
	curandSetPseudoRandomGeneratorSeed(generator,time(0));
	curandGenerateUniformDouble(generator,temp, len);
	curandDestroyGenerator(generator);

	size_t nBlocks = len/MAX_THREADS + 1;

	_MonteCarloTest<<<nBlocks,MAX_THREADS>>>(temp,len);
	cudaDeviceSynchronize();


	cudaMemcpy(*target, temp, len*sizeof(double),cudaMemcpyDeviceToHost);

	cudaFree(temp);
	
}*/