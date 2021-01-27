#pragma once
#include <stdio.h>

namespace CudaStuff {

	//Utilities
	extern void* ToCuda(void* data, size_t len, size_t byteSize);
	extern void UnCuda(void* data);
	extern void ToHost(void* data, size_t len, size_t byteSize, void* dst);
	extern double* CudaInit(size_t n);

	//Double Vector operations
	extern void dVectorAdd(double* dst, double* src1, double* src2, size_t len);
	extern void dVectorSub(double* dst, double* src1, double* src2, size_t len);
	extern void dVectorHadamard(double* dst, double* src1, double* src2, size_t len);
	extern double dVectorDot(double* src1, double* src2, size_t len);
	extern void dVectorAssign(double* dst, double val, size_t len);

};
