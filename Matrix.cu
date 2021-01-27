#include "LinearAlgebra.h"

#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "CudaStuff.h"
//#include "Matrix.h"
#include <stdio.h>


__global__ // Multiplies A*B, where A nAxm is row major and B mxmB is col-major dst row-major nAxmB
void _cudaMatrixMult(NUMBER* dst, NUMBER* A, NUMBER* B, size_t nA,size_t m,size_t mB){
    size_t rowOffset = (blockIdx.x*blockDim.x +threadIdx.x);
	size_t colOffset = (blockIdx.y*blockDim.y +threadIdx.y);
	if(rowOffset < nA && colOffset < mB){
		size_t offsetA = rowOffset*m;
		size_t offsetB = colOffset*m;
		size_t offsetDst = rowOffset*mB + colOffset;
		dst[offsetDst]=0;
		for(size_t i=0;i< m;i++){
			dst[offsetDst] += A[offsetA + i]*B[offsetB+i];
		}
	}
	
}

int cudaMatrixMult(NUMBER* dst, NUMBER* A, NUMBER* B, size_t nA,size_t m,size_t mB){
	
	int nXBlocks = nA/32 + ((nA%32)!=0);
	int nYBlocks = mB/32 + ((mB%32)!=0);

	dim3 blocks(nXBlocks,nYBlocks);
	dim3 threads(32,32);

	_cudaMatrixMult<<<blocks,threads>>>(dst,A,B,nA,m,mB);
	return 0;
}