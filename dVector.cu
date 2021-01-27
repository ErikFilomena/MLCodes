#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "VectorCuda.h"
#include "CudaStuff.h"

#include <stdio.h>
#include <stdlib.h>
#include <cuda.h>
#include <curand.h>



//DEVICE FUNCTIONS
__global__
KERNEL _dVectorAdd(double* dst, double* src1, double* src2, size_t len){
    size_t index = blockIdx.x*blockDim.x + threadIdx.x;
    if(index<len){
        dst[index] = src1[index]+src2[index];
    }
}

__global__
KERNEL _dVectorSub(double* dst, double* src1, double* src2, size_t len){
    size_t index = blockIdx.x*blockDim.x + threadIdx.x;
    if(index<len){
        dst[index] = src1[index]-src2[index];
    }
}

__global__
KERNEL _dVectorHadamard(double* dst, double* src1, double* src2, size_t len){
    size_t index = blockIdx.x*blockDim.x + threadIdx.x;
    if(index<len){
        dst[index] = src1[index]*src2[index];
    }
}

__global__
KERNEL _dVectorDot(double* result, double* src1, double* src2, size_t len){
    size_t index = blockIdx.x*blockDim.x + threadIdx.x;
    if(index<len){
        double val = src1[index]*src2[index];
        atomicAdd(result,val);
    }
}

__global__
KERNEL _dVectorAssign(double* dst, double val, size_t len){
    size_t index = blockIdx.x*blockDim.x + threadIdx.x;
    if(index<len){
        dst[index] = val;
    }
}




//HOST WRAPPERS
void CudaStuff::dVectorAdd(double* dst, double* src1, double* src2, size_t len){
    int nBlocks = len/MAX_THREADS + 1; 
     _dVectorAdd<<<nBlocks,MAX_THREADS>>>(dst,src1,src2,len);
}

void CudaStuff::dVectorSub(double* dst, double* src1, double* src2, size_t len){
    int nBlocks = len/MAX_THREADS + 1; 
     _dVectorSub<<<nBlocks,MAX_THREADS>>>(dst,src1,src2,len);
}


void CudaStuff::dVectorHadamard(double* dst, double* src1, double* src2, size_t len){
    int nBlocks = len/MAX_THREADS + 1; 
     _dVectorHadamard<<<nBlocks,MAX_THREADS>>>(dst,src1,src2,len);
}

double CudaStuff::dVectorDot(double* src1, double* src2, size_t len){
    int nBlocks = len/MAX_THREADS + 1; 
    double* ptrResult;
    double result;
    cudaMalloc(&ptrResult,sizeof(double));
    _dVectorDot<<<nBlocks,MAX_THREADS>>>(ptrResult,src1,src2,len);
    cudaMemcpy(&result,ptrResult,sizeof(double),cudaMemcpyDeviceToHost);
    cudaFree(ptrResult);
    return result;
}

void* CudaStuff::ToCuda(void* data, size_t len, size_t byteSize){
	void* temp;
	size_t nBytes =  len*byteSize ;
	cudaMalloc( &temp, nBytes );
	cudaMemcpy(temp,data,nBytes,cudaMemcpyHostToDevice);
	return temp;
}

void CudaStuff::ToHost(void* data, size_t len, size_t byteSize, void* dst){
	cudaMemcpy(dst,data,len*byteSize,cudaMemcpyDeviceToHost);
}

void CudaStuff::UnCuda(void* data){
	cudaFree(data);
}

double* CudaStuff::CudaInit(size_t n){
    double* temp;
    cudaMallocManaged(&temp, n*sizeof(double));
    return temp;
}

void CudaStuff::dVectorAssign(double* dst, double val, size_t len){
    size_t nBlocks = len/MAX_THREADS + 1; 
    _dVectorAssign<<<nBlocks,MAX_THREADS>>>(dst,val,len);
}
